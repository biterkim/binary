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

typedef struct _friend {
	char id[255];
	char name[255];
	char gender[50];
	unsigned char age[50];
} friends;

items itemList[6] = { {"BOMB",0,0},{"POTION",0,1},{"CURE",0,2},{"BOOK",0,3}, {"SHIELD",0,4}, {"CANNON",0,5} };

typedef struct _User {

	char id[255], name[255];
	unsigned char age[50], hp[50], mp[50];
	unsigned short coin;
	int gender_i;
	char descript[1000];
	friends friend_list[100];


}User;


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
	User* user = (User*)malloc(sizeof(User));
	char* temp;
	char temp2[255] = "";
	char descript[1000] = { 0 };
	char crc;
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
	user->gender_i = temp_len;
	fread(&user->age, 1, 1, fp);
	fread(&user->hp, 1, 1, fp);
	fread(&user->mp, 1, 1, fp);
	fread(&user->coin, 2, 1, fp);


	int itemNum[6] = { 0 };
	fread(&item_count, 1, 1, fp);
	for (int i = 0; i < item_count; i++)
	{
		fread(&itemNum[i], 1, 1, fp);
		fread(&temp_len, 1, 1, fp);
		itemList[itemNum[i]].cnt = temp_len;
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
		if (temp_len) strcpy(user->friend_list[i].gender, "FEMALE");
		else strcpy(user->friend_list[i].gender, "MALE");

		fread(&user->friend_list[i].age, 1, 1, fp);
	}

	strcpy(descript, "");
	fread(descript, 1, 1024, fp);
	strcpy(user->descript, text_decode(descript));

	fclose(fp);


	fp2 = fopen("result.txt", "w+b");
	if (fp == NULL) {
		fprintf(stderr, "파일 오류\n");
		return 0;
	}
	//char* temp;

	fprintf(fp2, "*USER STATUS*\n");
	fprintf(fp2, "ID: %s\n", user->id);
	fprintf(fp2, "NAME: %s\n", user->name);
	if (user->gender_i == 0) {
		fprintf(fp2, "GENDER: MALE\n");
	}
	else if (user->gender_i == 1) {
		fprintf(fp2, "GENDER: FEMALE\n");
	}

	fprintf(fp2, "AGE: %d\n", user->age[0]);
	fprintf(fp2, "HP: %d\n", user->hp[0]);
	fprintf(fp2, "MP: %d\n", user->mp[0]);
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
		fprintf(fp2, "FRIEND%d AGE: %d\n", i + 1, user->friend_list[i].age[0]);
		fprintf(fp2, "\n");

	}
	fprintf(fp2, "*DESCRIPTION*\n");
	fprintf(fp2, "%s", user->descript);



	fclose(fp2);

	printf("%s\n", user->id);
	printf("%s\n", user->name);
	printf("%d\n", user->gender_i);
	printf("%d\n", user->age[0]);
	printf("%d\n", user->hp[0]);
	printf("%d\n", user->mp[0]);
	printf("%d\n", user->coin);

	for (int i = 0; i < item_count; i++)
		printf("%s %d\n", itemList[itemNum[i]].itemName, itemList[itemNum[i]].cnt);


	for (int i = 0; i < friend_count; i++)
	{
		printf("%s\n", user->friend_list[i].id);
		printf("%s\n", user->friend_list[i].name);
		printf("%s\n", user->friend_list[i].gender);
		printf("%d\n", user->friend_list[i].age[0]);
	}
	printf("%s\n", user->descript);

	//printf("%s\n", descript);

	free(user);
	return 0;
}