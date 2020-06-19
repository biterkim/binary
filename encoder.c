#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//crc-16 계산
char* calculateCRC(char* crcdata, int len) {	//CRC-16-CCITT(xmodem)
    int i, j;
    unsigned short crc = 0;

    for (i = 0; i < len; i++) {
        crc = crc ^ ((*crcdata) << 8);
        crcdata++;
        for (j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;   //정방향 0x1021 0001 0000 0010 0000
            }
            else {                            //역방향 0x8408 1000 0100 0000 1000
                crc = crc << 1;
            }
        }
    }
    return (char*)crc;

}

char* text_encode(char* p1)
{
    char* re1 = strtok(p1, "\n");
    char* re2;
    int re1_len = strlen(re1), text_count = 1;
    re2 = (char*)malloc(sizeof(char) * re1_len);
    strcpy(re2, "");
    for (int i = 0; i < re1_len; i++)
    {
        if (re1[i] == re1[i + 1])   text_count++;
        else
        {
            if (text_count == 1) sprintf(re2, "%s%c", re2, re1[i]);
            else if (text_count == 2)
            {
                sprintf(re2, "%s%c%c", re2, re1[i - 1], re1[i]);
                text_count = 1;
            }
            else {
                sprintf(re2, "%s*%c%x", re2, re1[i], text_count);
                text_count = 1;
            }
        }
    }
    return re2;
}

// user 정보 구조체
typedef struct _Info {
	char id[255], name[255], gender[50], age[50], hp[50], mp[50], coin[50];
	char descript[1000];
}Info;

//친구 구조체
typedef struct _friend {
	char id[255];
	char name[255];
	char gender[50];
	char age[50];
} friends;

// 친구 구조체 초기화
friends* init_friends() {
	friends* friend_list = (friends*)malloc(sizeof(friends) * 100);
	return friend_list;
}

//item 구조체
typedef struct _item {
	char itemName[255];
	int cnt;
	int itemNum;
} items;
//item 번호 변환
int changeItemNum(items* itemList, const char* itemName) {
	int i = 0;

	while (i < 6) {
		int value = strcmp(itemList[i].itemName, itemName);
		if (value == 0) {
			return itemList[i].itemNum;
		}
		else {
			i++;
			continue;
		}
	}
	printf("값이 없습니다.\n");
	exit(1);
	return itemList[i].itemNum;
}

// 아이템 구조체 초기화
items* init_itemList() {
	items* itemList = (items*)malloc(sizeof(items) * 6);
	strcpy(itemList[0].itemName, "BOMB");
	itemList[0].itemNum = 0;
	itemList[0].cnt = 0;

	strcpy(itemList[1].itemName, "POTION");
	itemList[1].itemNum = 1;
	itemList[1].cnt = 0;

	strcpy(itemList[2].itemName, "CURE");
	itemList[2].itemNum = 2;
	itemList[2].cnt = 0;

	strcpy(itemList[3].itemName, "BOOK");
	itemList[3].itemNum = 3;
	itemList[3].cnt = 0;

	strcpy(itemList[4].itemName, "SHIELD");
	itemList[4].itemNum = 4;
	itemList[4].cnt = 0;

	strcpy(itemList[5].itemName, "CANNON");
	itemList[5].itemNum = 5;
	itemList[5].cnt = 0;

	return itemList;
}

//파일 불러오기
FILE* file_open(const char* fileName,const char* rwa) {
	FILE* fp = fopen(fileName, rwa);
	if (fp == NULL) {
		fprintf(stderr, "파일 오류\n");
		exit(1);
	}
	return fp;
}

// 글자수 예외처리
void check_strlen(const char*kind,const char* str,int count) {
	if (strlen(str) > count) {
		printf("%s의 글자수가 %d자를 넘었습니다.[%d글자]\n",kind,count,strlen(str));
		exit(1);
	}
}
// 숫자 예외처리
void check_int(const char*kind, const char* str, int count) {
	if (atoi(str) < 0 || atoi(str) > count) {
		printf("%s가 0~%d이외의 값입니다!!\n",kind,count);
		exit(1);
	}
}
void check_int2(const char*kind, int what, int count) {
	if (what < 0 || what > count) {
		printf("%s가 %d명을 초과 했습니다!!\n", kind, count);
		exit(1);
	}
}

//전체 출력
void print_all(Info* info, int item_count, int itemNum[6], items* itemList,int friend_count ,friends*friend_list) {
	printf("%s\n", info->id);
	printf("%s\n", info->name);
	printf("%s\n", info->gender);
	printf("%s\n", info->age);
	printf("%s\n", info->hp);
	printf("%s\n", info->mp);
	printf("%s\n", info->coin);

	for (int i = 0; i < item_count; i++)
		printf("%s %d\n", itemList[itemNum[i]].itemName, itemList[itemNum[i]].cnt);


	for (int i = 0; i < friend_count; i++)
	{
		printf("%s\n", friend_list[i].id);
		printf("%s\n", friend_list[i].name);
		printf("%s\n", friend_list[i].gender);
		printf("%s\n", friend_list[i].age);
	}

	printf("%s\n", info->descript);
}

int main(int argc, char* argv[])
{
	items* itemList = init_itemList();
	friends* friend_list = init_friends();
	Info* info = (Info*)malloc(sizeof(Info));

	char* section;
	char* crc;
	int gender_i = 0;
    int temp_len = 0;
    int item_count = 0, friend_count = 0, text_count = 1;
    char descript[1000];
    FILE* fp, * fp2; //파일 디스크립터, fp2는 수정용 임시 파일제작

    char buf[1000];
	
	if(argc != 3){
        printf("usage) %s sample_file encode_file\n", argv[0]);
        exit(1);
    }

    fp = fopen(argv[1], "r+b");
    if (fp == NULL) {
        fprintf(stderr, "파일 오류\n");
        return 0;
    }
    fp2 = fopen(argv[2], "w+b");
    if (fp == NULL) {
        fprintf(stderr, "파일 오류\n");
        return 0;
    }
    fgets(buf, sizeof(buf), fp);
	fgets(buf, sizeof(buf), fp);
    sscanf(buf, "ID: %s", info->id);
	check_strlen("ID", info->id, 255);//id길이 예외

    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "NAME: %[^\n]", info->name);
	check_strlen("NAME", info->name, 255);//이름 길이 예외

    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "GENDER: %s", info->gender);
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "AGE: %s", info->age);
	check_int("AGE", info->age, 99); // 나이 예외처리

    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "HP: %s", info->hp);
	check_int("HP", info->hp, 255); // HP예외처리

    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "MP: %s", info->mp);
	check_int("MP", info->mp, 255); // MP예외처리

    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "COIN: %s", info->coin);
	check_int("COIN", info->coin, 65535);// COIN예외처리
    
	// text2파일에 쓰기(섹션1(본문))
	for (int k = 0; k < 3; k++)
    {
        temp_len = strlen(info->id);
        fwrite(&temp_len, 1, 1, fp2);
        fwrite(&info->id, sizeof(char), strlen(info->id), fp2);

        temp_len = strlen(info->name);
        fwrite(&temp_len, 1, 1, fp2);
        fwrite(&info->name, sizeof(char), strlen(info->name), fp2);

        if (info->gender == "MALE") gender_i = 0;
        else gender_i = 100;
        temp_len = atoi(info->age) + gender_i;
        fwrite(&temp_len, 1, 1, fp2);

        temp_len = atoi(info->hp);
        fwrite(&temp_len, 1, 1, fp2);

        temp_len = atoi(info->mp);
        fwrite(&temp_len, 1, 1, fp2);

        temp_len = atoi(info->coin);
        fwrite(&temp_len, 2, 1, fp2);
    }    

	// text2파일에 쓰기(섹션1(CRC))
    section = (char*)malloc(sizeof(char) * (strlen(info->id) + strlen(info->name) + 7));
    sprintf(section, "%d%s%d%s%d%d%d%d", strlen(info->id), info->id, strlen(info->name), info->name, atoi(info->age) + gender_i, atoi(info->hp), atoi(info->mp), atoi(info->coin));
    crc = calculateCRC(section, strlen(section));
    fwrite(&crc, 2, 1, fp2);
    fwrite(&crc, 2, 1, fp2);

	//섹션2
    fgets(buf, sizeof(buf), fp);
    fgets(buf, sizeof(buf), fp);
    int itemNum[6] = { 0 };
	int item_count_test = 0;
    while (fgets(buf, sizeof(buf), fp))
    {
        if (buf[0] != '\n')
        {
            char itemName[255];
            char itemcnt[10];
            sscanf(buf, "%[^:]:%s", itemName, itemcnt);
            item_count++;
            itemNum[item_count - 1] = changeItemNum(itemList, itemName);
            itemList[itemNum[item_count - 1]].cnt = atoi(itemcnt);

			//아이템 갯수 예외
			item_count_test += atoi(itemcnt);
			if (item_count_test > 255) {
				printf("item 갯수가 255개 초과입니다.\n");
				exit(1);
			}
        }
        else
        {
            break;
        }
    }
    //text파일 쓰기(섹션2(본문))
    for (int k = 0; k < 3; k++) {
        fwrite(&item_count, 1, 1, fp2);
        for (int i = 0; i < item_count; i++) {
            temp_len = itemList[itemNum[i]].cnt;
            fwrite(&itemNum[i], 1, 1, fp2);
            fwrite(&temp_len, 1, 1, fp2);
        }
    }
	//text2파일쓰기 (섹션2(CRC))
    section = (char*)malloc(sizeof(char) * 1 + (item_count * 2));
    sprintf(section, "%d", item_count);
    for (int i = 0; i < item_count; i++) {
        temp_len = itemList[itemNum[i]].cnt;
        sprintf(section, "%s%d%d", section, itemNum[i], temp_len);
    }
    crc = calculateCRC(section, strlen(section));
    fwrite(&crc, 2, 1, fp2);
    fwrite(&crc, 2, 1, fp2);
	
	//세션 3
    fgets(buf, sizeof(buf), fp);
	
    while (fgets(buf, sizeof(buf), fp))
    {
        if (buf[0] == 'F')
        {
            friend_count++;
            sscanf(buf, "%*[^:]:%s", friend_list[friend_count - 1].id);
            fgets(buf, sizeof(buf), fp);

            sscanf(buf, "%*[^:]: %[^\n]", friend_list[friend_count - 1].name);
            fgets(buf, sizeof(buf), fp);

            sscanf(buf, "%*[^:]:%s", friend_list[friend_count - 1].gender);
            fgets(buf, sizeof(buf), fp);

            sscanf(buf, "%*[^:]:%s", friend_list[friend_count - 1].age);

            fgets(buf, sizeof(buf), fp);
        }
        else
        {
            break;
        }
    }
	check_int2("친구", friend_count, 100);

	//text2 파일쓰기 (섹션3(본문))
    for (int k = 0; k < 3; k++) {
        fwrite(&friend_count, 1, 1, fp2);
        for (int i = 0; i < friend_count; i++) {
            temp_len = strlen(friend_list[i].id);
            fwrite(&temp_len, 1, 1, fp2);
            fwrite(&friend_list[i].id, sizeof(char), temp_len, fp2);

            temp_len = strlen(friend_list[i].name);
            fwrite(&temp_len, 1, 1, fp2);
            fwrite(&friend_list[i].name, sizeof(char), temp_len, fp2);

            if (!strcmp(friend_list[i].gender, "MALE")) temp_len = 0;
            else temp_len = 100;

            temp_len += atoi(friend_list[i].age);
            fwrite(&temp_len, 1, 1, fp2);
        }
    }
    
	//text2 파일쓰기 (섹션3(CRC))
    section = (char*)malloc(sizeof(char) * 1 + (friend_count * (sizeof(friend_list[0].id) + sizeof(friend_list[0].name) + 3)));
    sprintf(section, "%d", friend_count);
    for (int i = 0; i < friend_count; i++) {
        if (!strcmp(friend_list[i].gender, "MALE")) temp_len = 0;
        else temp_len = 100;
        sprintf(section, "%s%d%s%d%s%d", section, strlen(friend_list[i].id), friend_list[i].id, strlen(friend_list[i].name), friend_list[i].name, temp_len + atoi(friend_list[i].age));
    }
    crc = calculateCRC(section, strlen(section));
    fwrite(&crc, 2, 1, fp2);
    fwrite(&crc, 2, 1, fp2);
        
	//섹션4
    section = (char*)malloc(sizeof(char) * 1000);
    strcpy(info->descript, "");
    strcpy(section, "");
    while (fgets(buf, sizeof(buf), fp))
    {
        sprintf(section, "%s%s", section, buf);
        if (buf[0] != '\n')
        {            
            temp_len = strlen(buf);
            for (int i = 0; i < temp_len; i++)
            {
                if (buf[i] == buf[i + 1])  text_count++; 
                else
                {
                    if (text_count == 1)
                    {
                        sprintf(info->descript, "%s%c", info->descript, buf[i]);
                    }
                    else if (text_count == 2)
                    {
                        sprintf(info->descript, "%s%c%c", info->descript, buf[i - 1], buf[i]);
                        text_count = 1;
                    }
                    else {
                        sprintf(info->descript, "%s*%c%c", info->descript, buf[i], text_count);
                        text_count = 1;
                    }
                }
            }
            //sprintf(descript, "%s\n", descript);
        }
        else
        {
            break;
        }
    }
	check_strlen("DESCRIPTION", info->descript, 1000);//description 크기 예외
	//text2 파일쓰기 (섹션3(본문))
    unsigned short descript_len = strlen(info->descript);
    for (int k = 0; k < 3; k++) {
        fwrite(&descript_len, 1, 2, fp2);
        fwrite(&info->descript, 1, strlen(info->descript), fp2);
    }
    sprintf(section, "%d%s", descript_len, info->descript);
    crc = calculateCRC(section, strlen(section));
    fwrite(&crc, 2, 1, fp2);
    fwrite(&crc, 2, 1, fp2);

    fclose(fp);
    fclose(fp2);

	printf("인코딩 된 데이터 확인.\n");
	print_all(info, item_count, itemNum, itemList, friend_count, friend_list);
	printf("인코딩 완료.\n");
	
	free(itemList);
	free(friend_list);
	free(info);

    return 0;
}
