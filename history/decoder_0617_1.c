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

int crc_check(unsigned short crc, unsigned short c_crc[4])
{
	int notEqual = 1;   //0다르다 1 같다
	int count = 0;

	for (int i = 0; i < 4; i++) {
		if (!(crc == c_crc[i]))
		{
			notEqual = 0;
			count++;
		}
	}

	if (notEqual == 0 && count > 2)//원본 회손
	{
		return 1;
	}
	else if (notEqual == 0 && count == 2) //crc 2개 변조
	{
		return 2;
	}
	else if (notEqual == 0 && count == 1)//crc 1개 변조
	{
		return 3;
	}

	return 0;   //정상
}


int main() {
	Section1* section1[3];
	for (int i = 0; i < 3; i++) section1[i] = (Section1*)malloc(sizeof(Section1));

	Section2* section2[3];
	for (int i = 0; i < 3; i++) section2[i] = (Section2*)malloc(sizeof(Section2));

	Section3* section3[3];
	for (int i = 0; i < 3; i++) section3[i] = (Section3*)malloc(sizeof(Section3));

	Section4* section4[3];
	for (int i = 0; i < 3; i++) section4[i] = (Section4*)malloc(sizeof(Section4));

	char* temp;
	char temp2[255] = "";
	char descript[1000] = { 0 };
	char* section[3];
	char* sectiontemp;
	unsigned short crc;
	unsigned short crc_c[4];
	int temp_len = 0;
	int iserr = 0, err_code = 0;
	int item_count = 0, friend_count = 0, text_count = 1;

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
		fread(&section1[1]->crc_c[i], 2, 1, fp);
	}

	//섹션2 정보 가져오기
	int itemNum[3][6] = { 0 };
	for (int i = 0; i < 3; i++) {
		
		fread(&item_count, 1, 1, fp);
		for (int j = 0; j < item_count; j++)
		{
			fread(&itemNum[i][j], 1, 1, fp);
			fread(&temp_len, 1, 1, fp);
			itemList[i][itemNum[i][j]].cnt = temp_len;
		}
	}
	for (int i = 0; i < 2; i++) {
		fread(&section2[1]->crc_c[i], 2, 1, fp);
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
		fread(&section3[1]->crc_c[i], 2, 1, fp);
	}

	//섹션4 정보 가져오기
	for (int i = 0; i < 3; i++) {
		fread(&temp_len, 1, 2, fp);
		fread(&descript, 1, temp_len, fp);
		strcpy(section4[i]->descript, descript);
	}
	for (int i = 0; i < 2; i++) {
		fread(&section4[1]->crc_c[i], 2, 1, fp);
	}

	fclose(fp);

	fp2 = fopen("result.txt", "w+b");
	if (fp == NULL) {
		fprintf(stderr, "파일 오류\n");
		return 0;
	}

	fprintf(fp2, "*USER STATUS*\n");
	fprintf(fp2, "ID: %s\n", section1[0]->id);
	fprintf(fp2, "NAME: %s\n", section1[0]->name);
	if (section1[0]->gender_i == 0) {
		fprintf(fp2, "GENDER: MALE\n");
	}
	else if (section1[0]->gender_i == 100) {
		fprintf(fp2, "GENDER: FEMALE\n");
	}

	fprintf(fp2, "AGE: %d\n", section1[0]->age);
	fprintf(fp2, "HP: %d\n", section1[0]->hp);
	fprintf(fp2, "MP: %d\n", section1[0]->mp);
	fprintf(fp2, "COIN: %d\n\n", section1[0]->coin);
	fprintf(fp2, "*ITEMS*\n");
	for (int i = 0; i < item_count; i++) {
		fprintf(fp2, "%s: %d\n", itemList[0][itemNum[0][i]].itemName, itemList[0][itemNum[0][i]].cnt);
	}
	fprintf(fp2, "\n");
	fprintf(fp2, "*FRIENDS LIST*\n");
	for (int i = 0; i < friend_count; i++)
	{
		fprintf(fp2, "FRIEND%d ID: %s\n", i + 1, section3[0]->friend_list[i].id);
		fprintf(fp2, "FRIEND%d NAME: %s\n", i + 1, section3[0]->friend_list[i].name);
		fprintf(fp2, "FRIEND%d GENDER: %s\n", i + 1, section3[0]->friend_list[i].gender);
		fprintf(fp2, "FRIEND%d AGE: %d\n", i + 1, section3[0]->friend_list[i].age);
		fprintf(fp2, "\n");

	}

	fprintf(fp2, "*DESCRIPTION*\n");
	fprintf(fp2, "%s", text_decode(section4[0]->descript));

	fclose(fp2);

	printf("%s\n", section1[0]->id);
	printf("%s\n", section1[0]->name);
	printf("%d\n", section1[0]->gender_i);
	printf("%d\n", section1[0]->age);
	printf("%d\n", section1[0]->hp);
	printf("%d\n", section1[0]->mp);
	printf("%d\n", section1[0]->coin);

	for (int i = 0; i < item_count; i++)
		printf("%s %d\n", itemList[0][itemNum[0][i]].itemName, itemList[0][itemNum[0][i]].cnt);


	for (int i = 0; i < friend_count; i++)
	{
		printf("%s\n", section3[0]->friend_list[i].id);
		printf("%s\n", section3[0]->friend_list[i].name);
		printf("%s\n", section3[0]->friend_list[i].gender);
		printf("%d\n", section3[0]->friend_list[i].age);
	}
	printf("%s", text_decode(section4[0]->descript));

	return 0;
}