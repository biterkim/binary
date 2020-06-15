#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

unsigned short calculateCRC(char* crcdata, int len) {	//CRC-16-CCITT(xmodem)
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

typedef struct _item {
	char itemName[255];
	int cnt;
	int itemNum;
} items;

typedef struct _friend {
	char id[255];
	char name[255];
	char gender[50];
	int age;
} friends;

items itemList[6] = { {"BOMB",0,0},{"POTION",0,1},{"CURE",0,2},{"BOOK",0,3}, {"SHIELD",0,4}, {"CANNON",0,5} };

typedef struct _User {

	char id[255], name[255];
	unsigned short coin;
	int gender_i, age, hp, mp;
	char descript[1000];
	friends friend_list[100];


}User;


char* text_decode(char* p1)
{
	char* re1 = strtok(p1, "\n");
	char *re2;
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
	int notEqual = 1;	//0다르다 1 같다
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

	return 0;	//정상
}

int main(int argc, char* argv[])
{
	User* user = (User*)malloc(sizeof(User));
	char* temp;
	char temp2[255] = "";
	char descript[1000] = { 0 };
	char* section;
	unsigned short crc;
	unsigned short crc_c[4];
	int temp_len = 0;
	int item_count = 0, friend_count = 0, text_count = 1;

	FILE* fp, * fp2; //파일 디스크립터, fp2는 수정용 임시 파일제작

	char buf[1000];

	fp = fopen("test2_sample.txt", "r+b");
	if (fp == NULL) {
		fprintf(stderr, "파일 오류\n");
		return 0;
	}


	fread(&temp_len, 1, 1, fp);
	memset(temp2, 0, 255);
	fread(temp2, 1, temp_len, fp);
	strcpy(user->id, temp2);
	fread(&temp_len, 1, 1, fp);
	memset(temp2, 0, 255);
	fread(temp2, 1, temp_len, fp);
	strcpy(user->name, temp2);
	fread(&temp_len, 1, 1, fp);
	if (temp_len < 100) {
		user->gender_i = 0;
		user->age=temp_len;
	}
	else {
		user->gender_i = 100;
		user->age = temp_len-100;
	}
	//user->gender_i = temp_len;
	//fread(&user->age, 1, 1, fp);
	fread(&user->hp, 1, 1, fp);
	fread(&user->mp, 1, 1, fp);
	fread(&user->coin, 2, 1, fp);

	section = (char*)malloc(sizeof(char) * (strlen(user->id) + strlen(user->name) + 7));
	sprintf(section, "%d%s%d%s%d%d%d%d", strlen(user->id), user->id, strlen(user->name), user->name, user->gender_i + user->age, user->hp, user->mp, user->coin);
	crc = calculateCRC(section, strlen(section));
	fread(&crc_c[0], 2, 1, fp);
	fread(&crc_c[1], 2, 1, fp);
	fread(&crc_c[2], 2, 1, fp);
	fread(&crc_c[3], 2, 1, fp);
	if (crc_check(crc, crc_c)) {
		printf("*USER STATUS* error\n");
	}
	

	int itemNum[6] = { 0 };
	fread(&item_count, 1, 1, fp);
	for (int i = 0; i < item_count; i++)
	{
		fread(&itemNum[i], 1, 1, fp);
		fread(&temp_len, 1, 1, fp);
		itemList[itemNum[i]].cnt = temp_len;
	}

	section = (char*)malloc(sizeof(char) * 1 + (item_count * 2));
	sprintf(section, "%d", item_count);
	for (int i = 0; i < item_count; i++) {
		temp_len = itemList[itemNum[i]].cnt;
		sprintf(section, "%s%d%d", section, itemNum[i], temp_len);
	}
	crc = calculateCRC(section, strlen(section));
	fread(&crc_c[0], 2, 1, fp);
	fread(&crc_c[1], 2, 1, fp);
	fread(&crc_c[2], 2, 1, fp);
	fread(&crc_c[3], 2, 1, fp);
	if (crc_check(crc, crc_c)) {
		printf("*ITEMS* error\n");
	}


	fread(&friend_count, 1, 1, fp);
	for (int i = 0; i < friend_count; i++) {
		fread(&temp_len, 1, 1, fp);
		memset(temp2, 0, 255);
		fread(temp2, 1, temp_len, fp);
		strcpy(user->friend_list[i].id, temp2);

		fread(&temp_len, 1, 1, fp);
		memset(temp2, 0, 255);
		fread(temp2, 1, temp_len, fp);
		strcpy(user->friend_list[i].name, temp2);

		fread(&temp_len, 1, 1, fp);
		if (temp_len > 99)
		{
			strcpy(user->friend_list[i].gender, "FEMALE");
			user->friend_list[i].age = temp_len-100;
		}
		else {
			strcpy(user->friend_list[i].gender, "MALE");
			user->friend_list[i].age = temp_len;
		}
		
		//fread(&user->friend_list[i].age, 1, 1, fp);
	}

	section = (char*)malloc(sizeof(char) * 1 + (friend_count * (sizeof(user->friend_list[0].id) + sizeof(user->friend_list[0].name) + 3)));
	sprintf(section, "%d", friend_count);
	for (int i = 0; i < friend_count; i++) {
		if (!strcmp(user->friend_list[i].gender, "MALE")) temp_len = 0;
		else temp_len = 100;
		sprintf(section, "%s%d%s%d%s%d", section, strlen(user->friend_list[i].id), user->friend_list[i].id, strlen(user->friend_list[i].name), user->friend_list[i].name, temp_len + user->friend_list[i].age);
	}
	crc = calculateCRC(section, strlen(section));
	fread(&crc_c[0], 2, 1, fp);
	fread(&crc_c[1], 2, 1, fp);
	fread(&crc_c[2], 2, 1, fp);
	fread(&crc_c[3], 2, 1, fp);
	if (crc_check(crc, crc_c)) {
		printf("*FRIENDS LIST* error\n");
	}
	
	//sprintf(descript,"");
	long n1 = ftell(fp);
	fseek(fp, -8, SEEK_END);
	long n2 = ftell(fp);
	fread(&crc_c[0], 2, 1, fp);
	fread(&crc_c[1], 2, 1, fp);
	fread(&crc_c[2], 2, 1, fp);
	fread(&crc_c[3], 2, 1, fp);
	fseek(fp, n1, SEEK_SET);
	strcpy(descript, "");
	fread(descript, 1, n2-n1, fp);
	strcpy(user->descript, text_decode(descript));
	crc = calculateCRC(user->descript, strlen(user->descript));	
	if (crc_check(crc, crc_c)) {
		printf("*DESCRIPTION* error\n");
	}
	fclose(fp);


	fp2 = fopen("result.txt", "w+b");
	if (fp == NULL) {
		fprintf(stderr, "파일 오류\n");
		return 0;
	}

	fprintf(fp2, "*USER STATUS*\n");
	fprintf(fp2, "ID: %s\n", user->id);
	fprintf(fp2, "NAME: %s\n", user->name);
	if (user->gender_i == 0) {
		fprintf(fp2, "GENDER: MALE\n");
	}
	else if (user->gender_i == 100) {
		fprintf(fp2, "GENDER: FEMALE\n");
	}

	fprintf(fp2, "AGE: %d\n", user->age);
	fprintf(fp2, "HP: %d\n", user->hp);
	fprintf(fp2, "MP: %d\n", user->mp);
	fprintf(fp2, "COIN: %d\n\n", user->coin);
	fprintf(fp2, "*ITEMS*\n");
	for (int i = 0; i < item_count; i++) {
		fprintf(fp2, "%s: %d\n", itemList[itemNum[i]].itemName, itemList[itemNum[i]].cnt);
	}
	fprintf(fp2, "\n");
	fprintf(fp2, "*FRIENDS LIST*\n");
	for (int i = 0; i < friend_count; i++)
	{
		fprintf(fp2, "FRIEND%d ID: %s\n", i + 1, user->friend_list[i].id);
		fprintf(fp2, "FRIEND%d NAME: %s\n", i + 1, user->friend_list[i].name);
		fprintf(fp2, "FRIEND%d GENDER: %s\n", i + 1, user->friend_list[i].gender);
		fprintf(fp2, "FRIEND%d AGE: %d\n", i + 1, user->friend_list[i].age);
		fprintf(fp2, "\n");

	}
	fprintf(fp2, "*DESCRIPTION*\n");
	fprintf(fp2, "%s", user->descript);



	fclose(fp2);

	printf("%s\n", user->id);
	printf("%s\n", user->name);
	printf("%d\n", user->gender_i);
	printf("%d\n", user->age);
	printf("%d\n", user->hp);
	printf("%d\n", user->mp);
	printf("%d\n", user->coin);

	for (int i = 0; i < item_count; i++)
		printf("%s %d\n", itemList[itemNum[i]].itemName, itemList[itemNum[i]].cnt);


	for (int i = 0; i < friend_count; i++)
	{
		printf("%s\n", user->friend_list[i].id);
		printf("%s\n", user->friend_list[i].name);
		printf("%s\n", user->friend_list[i].gender);
		printf("%d\n", user->friend_list[i].age);
	}
	printf("%s\n", user->descript);

	//printf("%s\n", descript);

	free(user);
	return 0;
}