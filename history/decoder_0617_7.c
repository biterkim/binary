#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// 세션1
typedef struct _Section1 {
	char id[255], name[255];
	unsigned short coin;
	int gender_i, age, hp, mp;
	unsigned short crc_c[2];
}Section1;

typedef struct _item {
	char itemName[255];
	int cnt;
	int itemNum;
} items;
items itemList[3][6] = { {{"BOMB",0,0},{"POTION",0,1},{"CURE",0,2},{"BOOK",0,3}, {"SHIELD",0,4}, {"CANNON",0,5}},
						{{"BOMB",0,0},{"POTION",0,1},{"CURE",0,2},{"BOOK",0,3}, {"SHIELD",0,4}, {"CANNON",0,5} },
						{{"BOMB",0,0},{"POTION",0,1},{"CURE",0,2},{"BOOK",0,3}, {"SHIELD",0,4}, {"CANNON",0,5} } };

//세션 2
typedef struct _Section2 {
	unsigned short crc_c[2];
}Section2;

//세션 3
typedef struct _friend {
	char id[255];
	char name[255];
	char gender[50];
	int age;
} friends;

typedef struct _Section3 {
	friends friend_list[100];
	unsigned short crc_c[2];
}Section3;

//세션4
typedef struct _Section4 {
	char descript[1000];
	unsigned short crc_c[2];
}Section4;

typedef struct _crc_s {
	unsigned short crc[3];
	unsigned short crc_c[2];
}crc_s;

unsigned short calculateCRC(char* crcdata, int len) {   //CRC-16-CCITT(xmodem)
	unsigned int i, j;
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
	return crc;

}

char* text_decode(char* p1)
{
	char* re1 = strtok(p1, "\n");
	char* re2;
	int re1_len = strlen(re1), text_count = 1;
	re2 = (char*)malloc(sizeof(char) * 1000);
	strcpy(re2, "");
	while (re1 != NULL)
	{
		re1_len = strlen(re1);
		for (int i = 0; i < re1_len; i++)
		{
			if (re1[i] != '*')
			{
				sprintf(re2, "%s%c", re2, re1[i]);
			}
			else {
				i++;
				for (int j = 0; j < (unsigned char)re1[i + 1]; j++)
				{
					sprintf(re2, "%s%c", re2, re1[i]);
				}
				i++;
			}
		}
		sprintf(re2, "%s\n", re2);
		re1 = strtok(NULL, "\n");
	}
	return re2;
}

typedef struct _crc_check_s {
	int success_count;
	int success;	
	int error_count;
	int error[2];
}crc_check_s;

crc_check_s crc_check(unsigned short crc[3], unsigned short c_crc[2])
{
	crc_check_s check = { 0,0,0,{0,0} };
	
	unsigned short crc_r[5];

	crc_r[0] = crc[0];
	crc_r[1] = crc[1];
	crc_r[2] = crc[2];
	crc_r[3] = c_crc[0];
	crc_r[4] = c_crc[1];
	for (int i = 0; i < 5; i++)
	{
		check.success_count =0;
		for (int j = 0; j < 5; j++)
		{
			if (crc_r[i] == crc_r[j])
				check.success_count++;
		}
		if (check.success_count > 2)
			check.success = i;
		else if(check.success_count < 3)
		{
			check.error[check.error_count] = i;
			check.error_count++;
		}			
	}	
	if (check.success_count >2)
	{
		for (int j = 0; j < 5; j++)
		{
			if (crc_r[check.success_count] == crc_r[j])
			{
				check.success = j;
				break;
			}				
		}
	}
	return check;
}

typedef struct _real_data {
	int is_error;
	int real_data;
	int error_data[2];
}real_data;

int main() {
	Section1* section1[3];
	for (int i = 0; i < 3; i++) section1[i] = (Section1*)malloc(sizeof(Section1));

	Section2* section2[3];
	for (int i = 0; i < 3; i++) section2[i] = (Section2*)malloc(sizeof(Section2));

	Section3* section3[3];
	for (int i = 0; i < 3; i++) section3[i] = (Section3*)malloc(sizeof(Section3));

	Section4* section4[3];
	for (int i = 0; i < 3; i++) section4[i] = (Section4*)malloc(sizeof(Section4));

	crc_s* crc_c[4];
	for (int i = 0; i < 4; i++) crc_c[i] = (crc_s*)malloc(sizeof(crc_s));
	char* temp;
	char temp2[255] = "";
	char descript[1000] = { 0 };
	char* crc_string;
	char* section[3];
	char* sectiontemp;
	unsigned short crc[3];
	//unsigned short crc_c[2];
	int temp_len = 0;
	int iserr = 0, err_code = 0;
	int item_count[3] = { 0 }, friend_count = 0, text_count = 1;

	FILE* fp, * fp2; //파일 디스크립터, fp2는 수정용 임시 파일제작

	char buf[1000];

	fp = fopen("test2_sample.txt", "r+b");
	if (fp == NULL) {
		fprintf(stderr, "파일 오류\n");
		return 0;
	}
	//섹션1 정보 가져오기
	for (int i = 0; i < 3; i++) {
		fread(&temp_len, 1, 1, fp);
		memset(temp2, 0, 255);
		fread(temp2, 1, temp_len, fp);
		strcpy(section1[i]->id, temp2);
		fread(&temp_len, 1, 1, fp);
		memset(temp2, 0, 255);
		fread(temp2, 1, temp_len, fp);
		strcpy(section1[i]->name, temp2);
		fread(&temp_len, 1, 1, fp);
		if (temp_len < 100) {
			section1[i]->gender_i = 0;
			section1[i]->age = temp_len;
		}
		else {
			section1[i]->gender_i = 100;
			section1[i]->age = temp_len - 100;
		}
		fread(&temp_len, 1, 1, fp);
		section1[i]->hp = temp_len;
		fread(&temp_len, 1, 1, fp);
		section1[i]->mp = temp_len;
		fread(&section1[i]->coin, 2, 1, fp);
	}

	for (int i = 0; i < 2; i++) {
		fread(&crc_c[0]->crc_c[i], 2, 1, fp);
	}

	//섹션2 정보 가져오기
	int itemNum[3][6] = { 0 };
	for (int i = 0; i < 3; i++) {

		fread(&item_count[i], 1, 1, fp);
		for (int j = 0; j < item_count[i]; j++)
		{
			fread(&itemNum[i][j], 1, 1, fp);
			fread(&temp_len, 1, 1, fp);
			itemList[i][itemNum[i][j]].cnt = temp_len;
		}
	}
	for (int i = 0; i < 2; i++) {
		fread(&crc_c[1]->crc_c[i], 2, 1, fp);
	}

	//섹션3 정보 가져오기
	for (int j = 0; j < 3; j++) {
		fread(&friend_count, 1, 1, fp);
		for (int i = 0; i < friend_count; i++) {
			fread(&temp_len, 1, 1, fp);
			memset(temp2, 0, 255);
			fread(temp2, 1, temp_len, fp);
			strcpy(section3[j]->friend_list[i].id, temp2);

			fread(&temp_len, 1, 1, fp);
			memset(temp2, 0, 255);
			fread(temp2, 1, temp_len, fp);
			strcpy(section3[j]->friend_list[i].name, temp2);

			fread(&temp_len, 1, 1, fp);
			if (temp_len > 99)
			{
				strcpy(section3[j]->friend_list[i].gender, "FEMALE");
				section3[j]->friend_list[i].age = temp_len - 100;
			}
			else {
				strcpy(section3[j]->friend_list[i].gender, "MALE");
				section3[j]->friend_list[i].age = temp_len;
			}
		}

	}
	for (int i = 0; i < 2; i++) {
		fread(&crc_c[2]->crc_c[i], 2, 1, fp);
	}

	//섹션4 정보 가져오기
	for (int i = 0; i < 3; i++) {
		fread(&temp_len, 1, 2, fp);
		fread(&descript, 1, temp_len, fp);
		strcpy(section4[i]->descript, descript);
	}
	for (int i = 0; i < 2; i++) {
		fread(&crc_c[3]->crc_c[i], 2, 1, fp);
	}

	fclose(fp);

	for (int i = 0; i < 3; i++)
	{
		crc_string = (char*)malloc(sizeof(char) * (strlen(section1[i]->id) + strlen(section1[i]->name) + 7));
		sprintf(crc_string, "%d%s%d%s%d%d%d%d", strlen(section1[i]->id), section1[i]->id, strlen(section1[i]->name), section1[i]->name, section1[i]->age + section1[i]->gender_i, section1[i]->hp, section1[i]->mp, section1[i]->coin);
		crc_c[0]->crc[i] = calculateCRC(crc_string, strlen(crc_string));

		crc_string = (char*)malloc(sizeof(char) * 1 + (item_count[i] * 2));
		sprintf(crc_string, "%d", item_count[i]);
		for (int j = 0; j < item_count[i]; j++) {
			sprintf(crc_string, "%s%d%d", crc_string, itemNum[i][j], itemList[i][itemNum[i][j]].cnt);
		}
		crc_c[1]->crc[i] = calculateCRC(crc_string, strlen(crc_string));

		crc_string = (char*)malloc(sizeof(char) * 1 + (friend_count * (sizeof(section3[i]->friend_list[i].id) + sizeof(section3[0]->friend_list[i].name) + 3)));
		sprintf(crc_string, "%d", friend_count);
		for (int j = 0; j < friend_count; j++) {
			if (!strcmp(section3[i]->friend_list[j].gender, "MALE")) temp_len = 0;
			else temp_len = 100;
			sprintf(crc_string, "%s%d%s%d%s%d", crc_string, strlen(section3[i]->friend_list[j].id), section3[i]->friend_list[j].id, strlen(section3[i]->friend_list[j].name), section3[i]->friend_list[j].name, temp_len + section3[i]->friend_list[j].age);
		}
		crc_c[2]->crc[i] = calculateCRC(crc_string, strlen(crc_string));

		crc_string = (char*)malloc(sizeof(char) * 1000);
		sprintf(crc_string, "%d%s", strlen(section4[i]->descript), section4[i]->descript);
		crc_c[3]->crc[i] = calculateCRC(crc_string, strlen(crc_string));
	}
	crc_check_s check = { 0 };
	real_data r_data[4] = { 0 };
	for (int i = 0; i < 4; i++) {
		printf("%d %d %d %d %d ", crc_c[i]->crc[0], crc_c[i]->crc[1], crc_c[i]->crc[2], crc_c[i]->crc_c[0], crc_c[i]->crc_c[1]);
		check = crc_check(crc_c[i]->crc, crc_c[i]->crc_c);
		if (check.success_count == 5)
		{
			switch (i)
			{
			case 0:
				printf("*USER STATUS* 섹션 에러없음.\n");
				break;
			case 1:
				printf("*ITEMS* 섹션 에러없음.\n");
				break;
			case 2:
				printf("*FRIENDS LIST* 섹션 에러없음.\n");
				break;
			case 3:
				printf("*DESCRIPTION* 섹션 에러없음.\n");
				break;
			default:
				break;
			}
		}else {
			if (check.error_count == 1) {
				r_data[i].is_error = 1;
				r_data[i].real_data = check.success;
				r_data[i].error_data[0] = check.error[0];
				if (check.error[0] > 3)
				{
					switch (i)
					{
					case 0:
						printf("*USER STATUS* 섹션 CRC가 변조됨\n");
						break;
					case 1:
						printf("*ITEMS* 섹션 CRC가 변조됨\n");
						break;
					case 2:
						printf("*FRIENDS LIST* 섹션 CRC가 변조됨\n");
						break;
					case 3:
						printf("*DESCRIPTION* 섹션 CRC가 변조됨\n");
						break;
					default:
						break;
					}
				}else {
					switch (i)
					{
					case 0:
						printf("*USER STATUS* 섹션 변조됨\n");
						break;
					case 1:
						printf("*ITEMS* 섹션 변조됨\n");
						break;
					case 2:
						printf("*FRIENDS LIST* 섹션 변조됨\n");
						break;
					case 3:
						printf("*DESCRIPTION* 섹션 변조됨\n");
						break;
					default:
						break;
					}
					printf("%d번 데이터로 복원합니다. (%d번 데이터 에러)\n", check.success, check.error[0]);
				}				
			}
			else if (check.error_count == 2) {
				r_data[i].is_error = 1;
				r_data[i].real_data = check.success;
				r_data[i].error_data[0] = check.error[0];
				r_data[i].error_data[1] = check.error[1];
				if (check.error[0] > 3)
				{
					switch (i)
					{
					case 0:
						printf("*USER STATUS* 섹션 CRC가 변조됨\n");
						break;
					case 1:
						printf("*ITEMS* 섹션 CRC가 변조됨\n");
						break;
					case 2:
						printf("*FRIENDS LIST* 섹션 CRC가 변조됨\n");
						break;
					case 3:
						printf("*DESCRIPTION* 섹션 CRC가 변조됨\n");
						break;
					default:
						break;
					}
				}
				if (check.error[1] > 3)
				{
					switch (i)
					{
					case 0:
						printf("*USER STATUS* 섹션 CRC가 변조됨\n");
						break;
					case 1:
						printf("*ITEMS* 섹션 CRC가 변조됨\n");
						break;
					case 2:
						printf("*FRIENDS LIST* 섹션 CRC가 변조됨\n");
						break;
					case 3:
						printf("*DESCRIPTION* 섹션 CRC가 변조됨\n");
						break;
					default:
						break;
					}
				}
				if (!(check.error[0] > 3 && check.error[1] > 3))
				{
					switch (i)
					{
					case 0:
						printf("*USER STATUS* 섹션 변조됨\n");
						break;
					case 1:
						printf("*ITEMS* 섹션 변조됨\n");
						break;
					case 2:
						printf("*FRIENDS LIST* 섹션 변조됨\n");
						break;
					case 3:
						printf("*DESCRIPTION* 섹션 변조됨\n");
						break;
					default:
						break;
					}
					printf("%d번 데이터로 복원합니다. (%d번, %d번 데이터 에러)\n", check.success, check.error[0], check.error[1]);
				}
			}
		}			
	}

	fp2 = fopen("result.txt", "w+b");
	if (fp == NULL) {
		fprintf(stderr, "파일 오류\n");
		return 0;
	}

	fprintf(fp2, "*USER STATUS*\n");
	fprintf(fp2, "ID: %s\n", section1[r_data[0].real_data]->id);
	fprintf(fp2, "NAME: %s\n", section1[r_data[0].real_data]->name);
	if (section1[r_data[0].real_data]->gender_i == 0) {
		fprintf(fp2, "GENDER: MALE\n");
	}
	else if (section1[r_data[0].real_data]->gender_i == 100) {
		fprintf(fp2, "GENDER: FEMALE\n");
	}

	fprintf(fp2, "AGE: %d\n", section1[r_data[0].real_data]->age);
	fprintf(fp2, "HP: %d\n", section1[r_data[0].real_data]->hp);
	fprintf(fp2, "MP: %d\n", section1[r_data[0].real_data]->mp);
	fprintf(fp2, "COIN: %d\n\n", section1[r_data[0].real_data]->coin);
	fprintf(fp2, "*ITEMS*\n");
	for (int i = 0; i < item_count[r_data[1].real_data]; i++) {
		fprintf(fp2, "%s: %d\n", itemList[r_data[1].real_data][itemNum[r_data[1].real_data][i]].itemName, itemList[r_data[1].real_data][itemNum[r_data[1].real_data][i]].cnt);
	}
	fprintf(fp2, "\n");
	fprintf(fp2, "*FRIENDS LIST*\n");
	for (int i = 0; i < friend_count; i++)
	{
		fprintf(fp2, "FRIEND%d ID: %s\n", i + 1, section3[r_data[2].real_data]->friend_list[i].id);
		fprintf(fp2, "FRIEND%d NAME: %s\n", i + 1, section3[r_data[2].real_data]->friend_list[i].name);
		fprintf(fp2, "FRIEND%d GENDER: %s\n", i + 1, section3[r_data[2].real_data]->friend_list[i].gender);
		fprintf(fp2, "FRIEND%d AGE: %d\n", i + 1, section3[r_data[2].real_data]->friend_list[i].age);
		fprintf(fp2, "\n");
	}

	fprintf(fp2, "*DESCRIPTION*\n");
	fprintf(fp2, "%s", text_decode(section4[r_data[3].real_data]->descript));

	fclose(fp2);

	printf("%s\n", section1[r_data[0].real_data]->id);
	printf("%s\n", section1[r_data[0].real_data]->name);
	printf("%d\n", section1[r_data[0].real_data]->gender_i);
	printf("%d\n", section1[r_data[0].real_data]->age);
	printf("%d\n", section1[r_data[0].real_data]->hp);
	printf("%d\n", section1[r_data[0].real_data]->mp);
	printf("%d\n", section1[r_data[0].real_data]->coin);

	for (int i = 0; i < item_count[r_data[1].real_data]; i++)
		printf("%s %d\n", itemList[r_data[1].real_data][itemNum[r_data[1].real_data][i]].itemName, itemList[r_data[1].real_data][itemNum[r_data[1].real_data][i]].cnt);


	for (int i = 0; i < friend_count; i++)
	{
		printf("%s\n", section3[r_data[2].real_data]->friend_list[i].id);
		printf("%s\n", section3[r_data[2].real_data]->friend_list[i].name);
		printf("%s\n", section3[r_data[2].real_data]->friend_list[i].gender);
		printf("%d\n", section3[r_data[2].real_data]->friend_list[i].age);
	}
	printf("%s", text_decode(section4[r_data[3].real_data]->descript));

	return 0;
}