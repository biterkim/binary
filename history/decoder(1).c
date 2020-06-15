#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct _item {
	char itemName[255];
	int cnt;
	int itemNum;
} items;

items itemList[6] = { {"BOMB",0,0},{"POTION",0,1},{"CURE",0,2},{"BOOK",0,3}, {"SHIELD",0,4}, {"CANNON",0,5} };

int changeItemNum(items* itemList, const char* itemName) {
	items itemResult;
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


typedef struct _friend {
	char id[255];
	char name[255];
	char gender[50];
	unsigned char age[50];
} friends;

char* text_decode(char* p1)
{
	char* re1 = strtok(p1, "\n");
	char re2[1000] = { 0 };
	int re1_len = strlen(re1), text_count = 1;
	//strcpy(re2, "");
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

int main(int argc, char* argv[])
{
	char id[255] = "", name[255] = "";
	unsigned char age[50] = "", hp[50] = "", mp[50] = "";
	unsigned short coin;
	friends friend_list[100] = { 0 };
	char* item[255];
	char* temp;
	char crc;
	int gender_i = 0;
	int temp_len = 0;
	int item_count = 0, friend_count = 0, text_count = 1;
	char descript[1000] = { 0 };
	FILE* fp, * fp2; //파일 디스크립터, fp2는 수정용 임시 파일제작

	char buf[1000];


	fp = fopen("test2_sample.txt", "r+b");
	if (fp == NULL) {
		fprintf(stderr, "파일 오류\n");
		return 0;
	}


	fread(&temp_len, 1, 1, fp);
	fread(&id, 1, temp_len, fp);
	fread(&temp_len, 1, 1, fp);
	fread(&name, 1, temp_len, fp);
	fread(&gender_i, 1, 1, fp);
	fread(&age, 1, 1, fp);
	fread(&hp, 1, 1, fp);
	fread(&mp, 1, 1, fp);
	fread(&coin, 2, 1, fp);

	
	int itemNum[6] = { 0 };
	fread(&item_count, 1, 1, fp);
	for (int i = 0; i < item_count; i++)
	{
		fread(&itemNum[i], 1, 1, fp);
		fread(&itemList[itemNum[i]].cnt, 1, 1, fp);
	}


	fread(&friend_count, 1, 1, fp);
	for (int i = 0; i < friend_count; i++) {
		fread(&temp_len, 1, 1, fp);
		fread(&friend_list[i].id, 1, temp_len, fp);

		fread(&temp_len, 1, 1, fp);
		fread(&friend_list[i].name, 1, temp_len, fp);

		fread(&temp_len, 1, 1, fp);
		if (temp_len) strcpy(friend_list[i].gender, "FEMALE");
		else strcpy(friend_list[i].gender, "MALE");

		fread(&friend_list[i].age, 1, 1, fp);
	}
	strcpy(descript, "");
	fread(&descript, 1, 1024, fp);
	strcpy(descript, text_decode(descript));

	fclose(fp);


	fp2 = fopen("result.txt", "w+b");
	if (fp == NULL) {
		fprintf(stderr, "파일 오류\n");
		return 0;
	}
	//char* temp;

	fprintf(fp2, "*USER STATUS*\n");
	fprintf(fp2, "ID: %s\n",id);
	fprintf(fp2, "NAME: %s\n",name);
	if (gender_i == 0) {
		fprintf(fp2, "GENDER: MALE\n");
	}
	else if (gender_i == 1) {
		fprintf(fp2, "GENDER: FEMALE\n");
	}
	
	fprintf(fp2, "AGE: %d\n",age[0]);
	fprintf(fp2, "HP: %d\n",hp[0]);
	fprintf(fp2, "MP: %d\n",mp[0]);
	fprintf(fp2, "COIN: %d\n\n",coin);
	fprintf(fp2, "*ITEMS*\n");
	for (int i = 0; i < item_count; i++) {
		fprintf(fp2, "%s: %d\n",itemList[itemNum[i]].itemName, itemList[itemNum[i]].cnt);
	}
	fprintf(fp2, "\n");
	fprintf(fp2, "*FRIENDS LIST*\n");
	for (int i = 0; i < friend_count; i++)
	{
		fprintf(fp2,"FRIEND%d ID: %s\n", i + 1, friend_list[i].id);
		fprintf(fp2,"FRIEND%d NAME: %s\n", i + 1, friend_list[i].name);
		fprintf(fp2,"FRIEND%d GENDER: %s\n", i + 1, friend_list[i].gender);
		fprintf(fp2,"FRIEND%d AGE: %d\n", i + 1, friend_list[i].age[0]);
		fprintf(fp2, "\n");

	}
	fprintf(fp2, "*DESCRIPTION*\n");
	fprintf(fp2, "%s", descript);
	


	fclose(fp2);

	//	printf("%s\n", id);
	//	printf("%s\n", name);
	//	printf("%d\n", gender_i);
	//	printf("%d\n", age[0]);
	//	printf("%d\n", hp[0]);
	//	printf("%d\n", mp[0]);
	//	printf("%d\n", coin);

	//for (int i = 0; i < item_count; i++)
	//	printf("%s %d\n", itemList[itemNum[i]].itemName, itemList[itemNum[i]].cnt);


	//for (int i = 0; i < friend_count; i++)
	//{
	//	printf("%s\n", friend_list[i].id);
	//	printf("%s\n", friend_list[i].name);
	//	printf("%s\n", friend_list[i].gender);
	//	printf("%d\n", friend_list[i].age[0]);
	//}
	//printf("%s\n", descript);

	//printf("%s\n", descript);


	return 0;
}