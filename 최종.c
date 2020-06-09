#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char GetCRC8(char crc, char ch)
{
    int i;

    for (i = 0; i < 8; i++) {
        if (crc & 0x80) {
            crc <<= 1;
            if (ch & 0x80) {
                crc = crc | 0x01;
            }
            else {
                crc = crc & 0xfe;
            }
            crc = crc ^ 0x107;
        }
        else {
            crc <<= 1;
            if (ch & 0x80) {
                crc = crc | 0x01;
            }
            else {
                crc = crc & 0xfe;
            }
        }
        ch <<= 1;
    }
    return (crc);
}

char crc_8(char* source, int number)
{
    int i;

    char crc = 0;

    for (i = 0; i < number; i++) {
        crc = GetCRC8(crc, *source);
        source++;
    }

    crc = GetCRC8(crc, 0x00);

    return crc;
}

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
    char age[50];
} friends;

char* pasing(char* p1)
{
    char* re1 = strtok(p1, ":");
    re1 = strtok(NULL, ":");
    re1 = strtok(re1, "\n");
    for (int i = 0; i < strlen(re1); i++)
        re1[i] = re1[i + 1];
    return re1;
}

char* passing2(char* p1) {
    char* re1 = strtok(p1, ":");
    return re1;
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
            if (text_count == 1) sprintf(re2, "%s%c", re2, re1[i]);//strcat(re2,re1[i]+"");
            else if (text_count == 2)
            {
                sprintf(re2, "%s%c%c", re2, re1[i - 1], re1[i]);
                /*strcat(re2, re1[i - 1] + "");
                strcat(re2, re1[i] + "");*/
                text_count = 1;
            }
            else {
                sprintf(re2, "%s*%c%x", re2, re1[i], text_count);
                //strcat(re2, "*"+re1[i]+text_count);
                text_count = 1;
            }
        }
    }
    return re2;
}

int main(int argc, char* argv[])
{
    char id[255], name[255], gender[50], age[50], hp[50], mp[50], coin[50];

    friends friend_list[100] = { 0 };
    char* item[255];
    char* temp;
    char crc;
    int gender_i = 0;
    int temp_len = 0;
    int item_count = 0, friend_count = 0, text_count = 1;
    char descript[1000];
    FILE* fp, * fp2; //파일 디스크립터, fp2는 수정용 임시 파일제작

    char buf[600];


    fp = fopen("test1_sample.txt", "r+b");
    if (fp == NULL) {
        fprintf(stderr, "파일 오류\n");
        return 0;
    }
    fp2 = fopen("test2_sample.txt", "w+b");
    if (fp == NULL) {
        fprintf(stderr, "파일 오류\n");
        return 0;
    }

    fgets(buf, sizeof(buf), fp);
    fgets(buf, sizeof(buf), fp);
    strcpy(id, pasing(buf));
    temp_len = strlen(id);
    fwrite(&temp_len, 1, 1, fp2);
    //fwrite(&temp_len, 1, 1, fp2);
    //fwrite(&temp_len, 1, 1, fp2);
    //temp = (char*)&temp_len;
    //crc = crc_8(temp, strlen(temp));
    //fwrite(&crc, sizeof(char), 1, fp2);
    fwrite(&id, sizeof(char), strlen(id), fp2);
    //fwrite(&id, sizeof(char), strlen(id), fp2);
    //fwrite(&id, sizeof(char), strlen(id), fp2);
    //crc = crc_8(id, strlen(id));
    //fwrite(&crc, sizeof(char), 1, fp2);

    fgets(buf, sizeof(buf), fp);
    strcpy(name, pasing(buf));
    temp_len = strlen(name);
    fwrite(&temp_len, 1, 1, fp2);
    //fwrite(&temp_len, 1, 1, fp2);
    //fwrite(&temp_len, 1, 1, fp2);
    //temp = (char*)&temp_len;
    //crc = crc_8(temp, strlen(temp));
    //fwrite(&crc, sizeof(char), 1, fp2);
    fwrite(&name, sizeof(char), strlen(name), fp2);
    //fwrite(&name, sizeof(char), strlen(name), fp2);
    //fwrite(&name, sizeof(char), strlen(name), fp2);
    //crc = crc_8(name, strlen(name));
    //fwrite(&crc, sizeof(char), 1, fp2);

    fgets(buf, sizeof(buf), fp);
    strcpy(gender, pasing(buf));
    if (gender == "MALE") temp_len = 0;
    else temp_len = 1;
    fwrite(&temp_len, 1, 1, fp2);

    fgets(buf, sizeof(buf), fp);
    strcpy(age, pasing(buf));
    temp_len = atoi(age);
    fwrite(&temp_len, 1, 1, fp2);

    fgets(buf, sizeof(buf), fp);
    strcpy(hp, pasing(buf));
    temp_len = atoi(hp);
    fwrite(&temp_len, 1, 1, fp2);

    fgets(buf, sizeof(buf), fp);
    strcpy(mp, pasing(buf));
    temp_len = atoi(mp);
    fwrite(&temp_len, 1, 1, fp2);

    fgets(buf, sizeof(buf), fp);
    strcpy(coin, pasing(buf));
    temp_len = atoi(coin);
    fwrite(&temp_len, 2, 1, fp2);

    fgets(buf, sizeof(buf), fp);
    fgets(buf, sizeof(buf), fp);
    int itemNum;
    while (fgets(buf, sizeof(buf), fp))
    {
        if (buf[0] != '\n')
        {
            char itemName[255];
            strcpy(itemName, buf);
            strcpy(itemName, passing2(itemName));
            item_count++;
            itemNum = changeItemNum(itemList, itemName);
            temp = pasing(buf);
            temp_len = strlen(temp);
            item[item_count - 1] = (char*)malloc(sizeof(char) * temp_len);
            strcpy(item[item_count - 1], temp);
            itemList[itemNum].cnt = atoi(temp);
        }
        else
        {
            break;
        }
    }
    fwrite(&item_count, 1, 1, fp2);
    for (int i = 0; i < 6; i++) {
        temp_len = itemList[i].cnt;
        fwrite(&temp_len, 1, 1, fp2);
    }

    fgets(buf, sizeof(buf), fp);

    while (fgets(buf, sizeof(buf), fp))
    {
        if (buf[0] == 'F')
        {
            friend_count++;

            temp = pasing(buf);
            strcpy(friend_list[friend_count - 1].id, temp);
            fgets(buf, sizeof(buf), fp);

            temp = pasing(buf);
            strcpy(friend_list[friend_count - 1].name, temp);
            fgets(buf, sizeof(buf), fp);

            temp = pasing(buf);
            strcpy(friend_list[friend_count - 1].gender, temp);
            fgets(buf, sizeof(buf), fp);

            temp = pasing(buf);
            strcpy(friend_list[friend_count - 1].age, temp);

            fgets(buf, sizeof(buf), fp);
        }
        else
        {
            break;
        }
    }

    fwrite(&friend_count, 1, 1, fp2);

    for (int i = 0; i < friend_count; i++) {
        temp_len = strlen(friend_list[i].id);
        fwrite(&temp_len, 1, 1, fp2);
        fwrite(&friend_list[i].id, sizeof(char), temp_len, fp2);

        temp_len = strlen(friend_list[i].name);
        fwrite(&temp_len, 1, 1, fp2);
        fwrite(&friend_list[i].name, sizeof(char), temp_len, fp2);

        if (friend_list[i].gender == "MALE") temp_len = 0;
        else temp_len = 1;
        fwrite(&temp_len, 1, 1, fp2);

        temp_len = atoi(friend_list[i].age);
        fwrite(&temp_len, 1, 1, fp2);
    }

    strcpy(descript, "");
    while (fgets(buf, sizeof(buf), fp))
    {
        if (buf[0] != '\n')
        {
            strcat(descript, text_encode(buf));
            temp_len = strlen(buf);
            for (int i = 0; i < temp_len; i++)
            {
                if (buf[i] == buf[i + 1])   text_count++;
                else
                {
                    if (text_count == 1)
                    {
                        fwrite(&buf[i], 1, 1, fp2);
                        //sprintf(temp, "%s%c", temp, buf[i]);//strcat(re2,re1[i]+"");
                    }
                    else if (text_count == 2)
                    {
                        //fwrite(&temp, 1, strlen(temp), fp2);
                        fwrite(&buf[i - 1], 1, 1, fp2);
                        fwrite(&buf[i], 1, 1, fp2);
                        //sprintf(temp, "%s%c%c", temp, buf[i - 1], buf[i]);
                        /*strcat(re2, re1[i - 1] + "");
                        strcat(re2, re1[i] + "");*/
                        text_count = 1;
                    }
                    else {
                        //fwrite(&temp, 1, strlen(temp), fp2);
                        fwrite("*", 1, 1, fp2);
                        fwrite(&buf[i], 1, 1, fp2);
                        fwrite(&text_count, 1, 1, fp2);
                        fwrite("\n", 1, 1, fp2);
                        //sprintf(temp, "%s*%c%x", temp, buf[i], text_count);
                        //strcat(re2, "*"+re1[i]+text_count);
                        text_count = 1;
                    }
                }
            }
            strcat(descript, "\n");
        }
        else
        {
            break;
        }
    }

    //temp_len = strlen(descript);
    //fwrite(&temp_len, 3, 1, fp2);
    //fwrite(&descript, sizeof(char), strlen(descript), fp2);

    fclose(fp);
    fclose(fp2);

    printf("%s\n", id);
    printf("%s\n", name);
    printf("%s\n", gender);
    printf("%s\n", age);
    printf("%s\n", hp);
    printf("%s\n", mp);
    printf("%s\n", coin);

    for (int i = 0; i < 6; i++) 
        printf("%s %d\n", itemList[i].itemName, itemList[i].cnt);


    for (int i = 0; i < friend_count; i++)
    {
        printf("%s\n", friend_list[i].id);
        printf("%s\n", friend_list[i].name);
        printf("%s\n", friend_list[i].gender);
        printf("%s\n", friend_list[i].age);
    }

    printf("%s\n", descript);

    return 0;
}