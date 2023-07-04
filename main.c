#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>

int is_student=-1;
int is_read;
char output[18];

typedef struct{
    int year;
    int month;
    int day;
} day_info;

day_info current_day;
int current_time;

typedef struct{
    char name[30];
    char family[30];
    char user_id[15];
    char password[15];
    char national_id_code[11];
    char birthdate[11];
    char gender[7];
    char type[8];
    int credit;
} user_info;

user_info current_user;

typedef struct{
    char id[10];
    char name[30];
    char location[50];
    int capacity;
    char type[8];
    char meal[7];
    int lunch_start;
    int lunch_end;
    int dinner_start;
    int dinner_end;
} self_info;

typedef struct{
    char id[10];
    char name[30];
    char type[8];
    int price;
} food_info;

typedef struct{
    char self_id[10];
    day_info date;
    char type[7];
    char food_id[10];
    int count;
    int food_price;
    int start;
    int end;
    char gender[7];
} meal_info;

typedef struct{
    char self_id[10];
    day_info date;
    char meal[7];
    char food_id[10];
    char user_id[15];
    char status[7];
    int food_price;
    int start;
    int end;
} reserve_info;

typedef struct{
    char self_id[10];
    day_info date;
    char meal[7];
    char food_id[10];
    int reserve_count;
    int taken_count;
} self_report_info;

typedef struct{
    char user_id[15];
    int amount;
    day_info date;
    int time;
    char type[15];
} credit_info;

typedef struct{
    char title[100];
    char content[500];
    day_info end;
} news_info;

int day_difference(day_info *day1, day_info *day2){
    int difference=360*(day1->year-day2->year)+30*(day1->month-day2->month)+(day1->day-day2->day);
    return difference;
}

void put_data(char *command_args, int d1, int d2, char data[][d2]){
    char *start;
    int count, i=0;
    for(int j=0; j<d1; j++){
        count=0;
        for(; command_args[i]!='\0'; i++){
            if(command_args[i]==':'){
                start=&command_args[i]+1;
                break;
            }
        }
        for(i++; ; i++){
            if(command_args[i]=='|' || command_args[i]=='\n' || command_args[i]=='\0'){
                count=&command_args[i]-start;
                break;
            }
        }
        strncpy(data[j], start, count);
        data[j][count]='\0';
    }
    return;
}

void set_time(char *command_args){
    char data[2][15];
    if(is_read==1){
        put_data(command_args, 2, 15, data);
        sscanf(data[0], "%d-%d-%d", &current_day.year, &current_day.month, &current_day.day);
        sscanf(data[1], "%d", &current_time);
    }
    else{
        printf("date: \n");
        scanf("%d-%d-%d", &current_day.year, &current_day.month, &current_day.day);
        printf("time: \n");
        scanf("%d", &current_time);
    }
    sprintf(output, "%s", "success");
    printf("%s\n", output);
    return;
}

void change_password(char *command_args){
    if(is_student==-1){
        sprintf(output, "%s", "not-found");
        printf("%s\n", output);
        return;
    }
    char data[3][15];
    if(is_read==1){
        put_data(command_args, 3, 15, data);
    }
    else{
        printf("user: \n");
        scanf("%s", data[0]);
        printf("old Pass: \n");
        scanf("%s", data[1]);
        printf("new Pass: \n");
        scanf("%s", data[2]);
    }
    FILE *f=fopen("user list.bin", "rb+");
    user_info user;
    long long fpp;
    while(1){
        fpp=ftell(f);
        fread(&user, sizeof(user_info), 1, f);
        if(strcmp(user.user_id, data[0])==0){
            if( strcmp(user.type, "student")==0 ||
                strcmp(user.type, "admin")==0){
                
                if(strcmp(user.password, data[1])==0){
                    strcpy(user.password, data[2]);
                    fseek(f, fpp, SEEK_SET);
                    fwrite(&user, sizeof(user_info), 1, f);
                    sprintf(output, "%s", "success");
                    current_user=user;
                    break;
                }
                else{
                    sprintf(output, "%s", "permission-denied");
                    break;
                }
            }
            else{
                sprintf(output, "%s", "not-found");
                break;
            }
        }
        if(feof(f)){
            sprintf(output, "%s", "not-found");
            break;
        }
    }
    fclose(f);
    printf("%s\n", output);
    return;
}

void self_report(reserve_info *reserve, char identifier, char new_self[]){
    FILE *f=fopen("self report.bin", "rb+");
    self_report_info self;
    long long fpp;
    while(1){
        fpp=ftell(f);
        fread(&self, sizeof(self_report_info), 1, f);
        if(strcmp(self.self_id, reserve->self_id)==0){
            if(day_difference(&reserve->date, &self.date)==0){
                if(strcmp(self.meal, reserve->meal)==0){
                    if(strcmp(self.food_id, reserve->food_id)==0){
                        if(identifier=='r'){
                            self.reserve_count++;
                        }
                        else if(identifier=='x'){
                            self.reserve_count--;
                        }
                        else if(identifier=='t'){
                            self.taken_count++;
                        }
                        else if(identifier=='c'){
                            self.reserve_count--;
                            rewind(f);
                            self_report_info self2;
                            long long fpp2;
                            while(1){
                                fpp2=ftell(f);
                                fread(&self2, sizeof(self_report_info), 1, f);
                                if(strcmp(self2.self_id, new_self)==0){
                                    if(day_difference(&reserve->date, &self2.date)==0){
                                        if(strcmp(self2.meal, reserve->meal)==0){
                                            if(strcmp(self2.food_id, reserve->food_id)==0){
                                                self2.reserve_count++;
                                                fseek(f, fpp2, SEEK_SET);
                                                fwrite(&self2, sizeof(self_report_info), 1, f);
                                                break;
                                            }
                                        }
                                    }
                                }
                                if(feof(f)){
                                    sprintf(self2.self_id, "%s", new_self);
                                    self2.date=reserve->date;
                                    sprintf(self2.meal, "%s", reserve->meal);
                                    sprintf(self2.food_id, "%s", reserve->food_id);
                                    self2.reserve_count=1;
                                    self2.taken_count=0;
                                    fwrite(&self2, sizeof(self_report_info), 1, f);
                                    break;
                                }
                            }
                        }
                        fseek(f, fpp, SEEK_SET);
                        fwrite(&self, sizeof(self_report_info), 1, f);
                        break;
                    }
                }
            }
        }
        if(feof(f)){
            sprintf(self.self_id, "%s", reserve->self_id);
            self.date=reserve->date;
            sprintf(self.meal, "%s", reserve->meal);
            sprintf(self.food_id, "%s", reserve->food_id);
            self.reserve_count=1;
            self.taken_count=0;
            fwrite(&self, sizeof(self_report_info), 1, f);
            break;
        }
    }
    fclose(f);
    return;
}

void print_food_report(){
    FILE *f=fopen("reserves.bin", "rb");
    reserve_info reserve;
    while(fread(&reserve, sizeof(reserve_info), 1, f)==1){
        if(strcmp(current_user.user_id, reserve.user_id)==0){
            if(strcmp(reserve.status, "taken")==0){
                FILE *s=fopen("self.bin", "rb");
                self_info self;
                char self_name[30];
                FILE *fo=fopen("food.bin", "rb");
                food_info food;
                char food_name[30];
                while(1){
                    fread(&self, sizeof(self_info), 1, s);
                    if(strcmp(self.id, reserve.self_id)==0){
                        strcpy(self_name, self.name);
                        break;
                    }
                }
                while(1){
                    fread(&food, sizeof(food_info), 1, fo);
                    if(strcmp(food.id, reserve.food_id)==0){
                        strcpy(food_name, food.name);
                        break;
                    }
                }
                fclose(s);
                fclose(fo);
                printf("self name: %s          date: %d-%d-%d\n", self_name, reserve.date.year, reserve.date.month, reserve.date.day);
                printf("meal: %s          food name: %s\n", reserve.meal, food_name);
            }
        }
    }
    fclose(f);
    return;
}

void credit_change(int amount, day_info *date, int time, char type[]){
    FILE *f=fopen("credit report.bin", "ab");
    credit_info cred;
    cred.amount=amount;
    cred.date=*date;
    cred.time=time;
    sprintf(cred.type, "%s", type);
    sprintf(cred.user_id, "%s", current_user.user_id);
    fwrite(&cred, sizeof(credit_info), 1, f);
    fclose(f);
    return;
}

void add_user(char *command_args){
    char data[8][30];
    FILE *f;
    user_info user;
    user_info check;
    if(is_read==1){
        put_data(command_args, 8, 30, data);
        strcpy(check.name, data[0]);
        strcpy(check.family, data[1]);
        strcpy(check.user_id, data[2]);
        strcpy(check.password, data[3]);
        strcpy(check.national_id_code, data[4]);
        strcpy(check.birthdate, data[5]);
        strcpy(check.gender, data[6]);
        strcpy(check.type, data[7]);
    }
    else{
        printf("name: \n");
        scanf("%s", &check.name);
        printf("family: \n");
        scanf("%s", &check.family);
        printf("id: \n");
        scanf("%s", &check.user_id);
        printf("pass: \n");
        scanf("%s", &check.password);
        printf("national id: \n");
        scanf("%s", &check.national_id_code);
        printf("birthdate: \n");
        scanf("%s", &check.birthdate);
        printf("gender: \n");
        scanf("%s", &check.gender);
        printf("type: \n");
        scanf("%s", &check.type);
    }
    check.credit=0;
    if(is_student==0){
        f=fopen("user list.bin", "rb+");
        while(1){
            fread(&user, sizeof(user_info), 1, f);
            if(strcmp(user.user_id, check.user_id)==0 && (user.type[0]=='s' || user.type[0]=='a')){
                sprintf(output, "%s", "permission-denied");
                break;
            }
            if(feof(f)){
                fwrite(&check, sizeof(user_info), 1, f);
                sprintf(output, "%s", "success");
                break;
            }        
        }
    }
    else if(is_student==-1){
        if(check.type[0]=='a'){
            f=fopen("user list.bin", "rb+");
            while(1){
                fread(&user, sizeof(user_info), 1, f);
                if(strcmp(user.user_id, check.user_id)==0 && (user.type[0]=='s' || user.type[0]=='a')){
                    sprintf(output, "%s", "permission-denied");
                    break;
                }
                if(feof(f)){
                    fwrite(&check, sizeof(user_info), 1, f);
                    sprintf(output, "%s", "success");
                    break;
                }        
            }
        }
        else if(check.type[0]=='s'){
            f=fopen("approvals.bin", "rb+");
            while(1){
                fread(&user, sizeof(user_info), 1, f);
                if(strcmp(user.user_id, check.user_id)==0){
                    sprintf(output, "%s", "permission-denied");
                    break;
                }
                if(feof(f)){
                    fwrite(&check, sizeof(user_info), 1, f);
                    sprintf(output, "%s", "success");
                    break;
                }        
            }
        }
    }
    printf("%s\n", output);
    fclose(f);
    return;
}

void create_files(){
    FILE *f;
    f=fopen("user list.bin", "ab");
    fclose(f);
    f=fopen("approvals.bin", "ab");
    fclose(f);
    f=fopen("credit report.bin", "ab");
    fclose(f);
    f=fopen("food.bin", "ab");
    fclose(f);
    f=fopen("self.bin", "ab");
    fclose(f);
    f=fopen("reserves.bin", "ab");
    fclose(f);
    f=fopen("self report.bin", "ab");
    fclose(f);
    f=fopen("news.bin", "ab");
    fclose(f);
    f=fopen("meals.bin", "ab");
    fclose(f);
    is_read=1;
    add_user("name:admin|family:admin|user-id:admin|password:admin|national-id-code:1111111111|birthdate:1111-11-11|gender:male|type:admin");
    is_read=0;
    return;
}

void add_self(char *command_args){
    if(is_student!=0){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    self_info self;
    char data[8][50];
    if(is_read==1){
        put_data(command_args, 8, 50, data);
        strcpy(self.id, data[0]);
        strcpy(self.name, data[1]);
        strcpy(self.location, data[2]);
        sscanf(data[3], "%d", &self.capacity);
        strcpy(self.type, data[4]);
        strcpy(self.meal, data[5]);
        if(strcmp(self.meal, "lunch")==0){
            sscanf(data[6], "%d-%d", &self.lunch_start, &self.lunch_end);
            sscanf("0-0", "%d-%d", &self.dinner_start, &self.dinner_end);
        }
        else if(strcmp(self.meal, "dinner")==0){
            sscanf(data[7], "%d-%d", &self.dinner_start, &self.dinner_end);
            sscanf("0-0", "%d-%d", &self.lunch_start, &self.lunch_end);
        }
        else{
            sscanf(data[6], "%d-%d", &self.lunch_start, &self.lunch_end);
            sscanf(data[7], "%d-%d", &self.dinner_start, &self.dinner_end);
        }
    }
    else{
        printf("id: \n");
        scanf("%s", &self.id);
        printf("name: \n");
        scanf("%s", &self.name);
        printf("location: \n");
        scanf("%s", &self.location);
        printf("capacity: \n");
        scanf("%d", &self.capacity);
        printf("type: \n");
        scanf("%s", &self.type);
        printf("meal: \n");
        scanf("%s", &self.meal);
        if(strcmp(self.meal, "lunch")==0){
            printf("lunch time: \n");
            scanf("%d-%d", &self.lunch_start, &self.lunch_end);
        }
        else if(strcmp(self.meal, "dinner")==0){
            printf("dinner time: \n");
            scanf("%d-%d", &self.dinner_start, &self.dinner_end);
        }
        else{
            printf("lunch time: \n");
            scanf("%d-%d", &self.lunch_start, &self.lunch_end);
            printf("dinner time: \n");
            scanf("%d-%d", &self.dinner_start, &self.dinner_end);
        }
    }
    FILE *f=fopen("self.bin", "ab");
    fwrite(&self, sizeof(self_info), 1, f);
    sprintf(output, "%s", "success");
    printf("%s\n", output);
    fclose(f);
    return;
}

void add_food(char *command_args){
    if(is_student!=0){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    food_info food;
    char data[4][30];
    if(is_read==1){
        put_data(command_args, 4, 30, data);
        strcpy(food.id, data[0]);
        strcpy(food.name, data[1]);
        strcpy(food.type, data[2]);
        sscanf(data[3], "%d", &food.price);
    }
    else{
        printf("id: \n");
        scanf("%s", &food.id);
        printf("name: \n");
        scanf("%s", &food.name);
        printf("type: \n");
        scanf("%s", &food.type);
        printf("price: \n");
        scanf("%d", &food.price);
    }
    FILE *f=fopen("food.bin", "ab");
    fwrite(&food, sizeof(food_info), 1, f);
    sprintf(output, "%s", "success");
    printf("%s\n", output);
    fclose(f);
    return;
}

void add_meal(char *command_args){
    if(is_student!=0){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    meal_info meal;
    char data[5][15];
    if(is_read==1){
        put_data(command_args, 5, 15, data);
        strcpy(meal.self_id, data[0]);
        sscanf(data[1], "%d-%d-%d", &meal.date.year, &meal.date.month, &meal.date.day);
        strcpy(meal.type, data[2]);
        strcpy(meal.food_id, data[3]);
        sscanf(data[4], "%d", &meal.count);
    }
    else{
        printf("self id: \n");
        scanf("%s", meal.self_id);
        printf("date: \n");
        scanf("%d-%d-%d", &meal.date.year, &meal.date.month, &meal.date.day);
        printf("type: \n");
        scanf("%s", meal.type);
        printf("food id: \n");
        scanf("%s", meal.food_id);
        printf("count: \n");
        scanf("%d", &meal.count);
    }
    FILE *s=fopen("self.bin", "rb");
    self_info self;
    while(1){
        fread(&self, sizeof(self_info), 1, s);
        if(strcmp(meal.self_id, self.id)==0){
            if(strcmp(self.type, "boyish")==0){
                sprintf(meal.gender, "%s", "male");
            }
            else{
                sprintf(meal.gender, "%s", "female");
            }
            if(strcmp(meal.type, "lunch")==0){
                meal.start=self.lunch_start;
                meal.end=self.lunch_end;
            }
            else{
                meal.start=self.dinner_start;
                meal.end=self.dinner_end;
            }
            FILE *fo=fopen("food.bin", "rb");
            food_info food;
            while(1){
                fread(&food, sizeof(food_info), 1, fo);
                if(strcmp(meal.food_id, food.id)==0){
                    meal.food_price=food.price;
                    sprintf(output, "%s", "success");
                    break;
                }
                if(feof(fo)){
                    sprintf(output, "%s", "not-found");
                    printf("%s\n", output);
                    return;
                }   
            }
            fclose(fo);
            break;
        }
        if(feof(s)){
            sprintf(output, "%s", "not-found");
            printf("%s\n", output);
            return;
        }
    }
    fclose(s);
    FILE *f=fopen("meals.bin", "ab");
    fwrite(&meal, sizeof(meal_info), 1, f);
    fclose(f);
    printf("%s\n", output);
    return;
}

void remove_user(char *command_args){
    if(is_student!=0){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    char data[1][15];
    if(is_read==1){
        put_data(command_args, 1, 15, data);
    }
    else{
        printf("id:\n");
        scanf("%s", data[0]);
    }
    FILE *f=fopen("user list.bin", "rb+");
    user_info user;
    long long int fpp;
    while(1){
        fpp=ftell(f);
        fread(&user, sizeof(user_info), 1, f);
        if(strcmp(user.user_id, data[0])==0 && user.type[0]!='r'){
            if(is_read==0){
                char c;
                printf("Are you sure? y/n\n");
                getchar();
                scanf("%c", &c);
                if(c=='n'){
                    sprintf(output, "%s", "no removal");
                    break;
                }
            }
            sprintf(user.type, "%s", "rem");
            sprintf(output, "%s", "success");
            fseek(f, fpp, SEEK_SET);
            fwrite(&user, sizeof(user_info), 1, f);
            break;
        }
        if(feof(f)){
            sprintf(output, "%s", "not-found");
            break;
        }        
    }
    printf("%s\n", output);
    fclose(f);
    return;
}

void deactivate_user(char *command_args){
    if(is_student!=0){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    char data[1][15];
    if(is_read==1){
        put_data(command_args, 1, 15, data);
    }
    else{
        printf("id:\n");
        scanf("%s", data[0]);
    }
    FILE *f=fopen("user list.bin", "rb+");
    user_info user;
    long long int fpp;
    while(1){
        fpp=ftell(f);
        fread(&user, sizeof(user_info), 1, f);
        if(strcmp(user.user_id, data[0])==0 && strcmp(user.type, "student")==0){
            FILE *out=fopen("approvals.bin", "rb+");
            user_info check;
            long long int fpp2;
            while(1){
                fpp2=ftell(out);
                fread(&check, sizeof(user_info), 1, out);
                if(strcmp(check.user_id, data[0])==0){
                    sprintf(check.type, "%s", "deact");
                    fseek(out, fpp2, SEEK_SET);
                    fwrite(&check, sizeof(user_info), 1, out);
                    break;
                }
                if(feof(out)){
                    sprintf(check.type, "%s", "deact");
                    fwrite(&check, sizeof(user_info), 1, out);
                    break;
                }
            }
            fclose(out);
            sprintf(user.type, "%s", "deact");
            sprintf(output, "%s", "success");
            fseek(f, fpp, SEEK_SET);
            fwrite(&user, sizeof(user_info), 1, f);
            break;
        }
        if(feof(f)){
            sprintf(output, "%s", "not-found");
            break;
        }        
    }
    printf("%s\n", output);
    fclose(f);
    return;
}

void login(char *command_args){
    char data[2][30];
    if(is_read==1){
        put_data(command_args, 2, 30, data);
    }
    else{
        printf("User: \n");
        scanf("%s", data[0]);
        printf("Pass: \n");
        int i=0;
        while(1){
            data[1][i]=(char)getch();
            if(data[1][i]=='\r'){
                break;
            }
            i++;
            putchar('*');
        }
        printf("\n");
        data[1][i]='\0';
    }
    FILE *f=fopen("user list.bin", "rb");
    user_info user;
    while(1){
        fread(&user, sizeof(user_info), 1, f);
        if(strcmp(user.user_id, data[0])==0 && (user.type[0]=='s' || user.type[0]=='a')){
            if(strcmp(user.password, data[1])==0){
                sprintf(output, "%s", "success");
                if(user.type[0]=='a'){
                    is_student=0;
                } else if(user.type[0]=='s'){
                    is_student=1;
                }
                current_user=user;
            } else{
                sprintf(output, "%s", "not-found");
            }
            break;
        }
        if(feof(f)){
            sprintf(output, "%s", "not-found");
            break;
        }        
    }
    printf("%s\n", output);
    fclose(f);
    return;
}

void logout(char *command_args){
    char data[1][15];
    if(is_read==1){
        put_data(command_args, 1, 15, data);
    }
    else{
        printf("User: \n");
        scanf("%s", data[0]);
    }
    if(is_student==-1){
        sprintf(output, "%s", "not-found");
    }
    else if(strcmp(current_user.user_id, data[0])==0){
        is_student=-1;
        FILE *f=fopen("user list.bin", "rb+");
        user_info user;
        long long fpp;
        while(1){
            fpp=ftell(f);
            fread(&user, sizeof(user_info), 1, f);
            if(strcmp(current_user.user_id, user.user_id)==0){
                user=current_user;
                fseek(f, fpp, SEEK_SET);
                fwrite(&user, sizeof(user_info), 1, f);
                
                sprintf(current_user.name, "%s", "\0");
                sprintf(current_user.family, "%s", "\0");
                sprintf(current_user.user_id, "%s", "\0");
                sprintf(current_user.password, "%s", "\0");
                sprintf(current_user.national_id_code, "%s", "\0");
                sprintf(current_user.gender, "%s", "\0");
                sprintf(current_user.birthdate, "%s", "\0");
                sprintf(current_user.type, "%s", "\0");
                current_user.credit=0;
                sprintf(output, "%s", "success");
                fclose(f);
                break;
            }
        }
    }
    else{
        sprintf(output, "%s", "not-found");
    }
    printf("%s\n", output);
    return;
}

void change_student_pass(char *command_args){
    if(is_student!=0){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    char data[2][15];
    if(is_read==1){
        put_data(command_args, 2, 15, data);
    }
    else{
        printf("user: \n");
        scanf("%s", data[0]);
        printf("new Pass: \n");
        scanf("%s", data[1]);
    }
    FILE *f=fopen("user list.bin", "rb+");
    user_info user;
    long long int fpp;
    while(1){
        fpp=ftell(f);
        fread(&user, sizeof(user_info), 1, f);
        if(strcmp(user.user_id, data[0])==0 && (user.type[0]=='s' || user.type[0]=='a')){
            sprintf(user.password, "%s", data[1]);
            sprintf(output, "%s", "success");
            fseek(f, fpp, SEEK_SET);
            fwrite(&user, sizeof(user_info), 1, f);
            break;
        }
        if(feof(f)){
            sprintf(output, "%s", "not-found");
            break;
        }        
    }
    printf("%s\n", output);
    fclose(f);
    return;
}

void approve(char *command_args){
    if(is_student!=0){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    char data[15];
    if(is_read==1){
        char *start;
        int count=0, i=0;
        for(; command_args[i]!='\0'; i++){
            if(command_args[i]==':'){
                start=&command_args[i]+1;
                break;
            }
        }
        for(i++; ; i++){
            if(command_args[i]=='|' || command_args[i]=='\n' || command_args[i]=='\0'){
                count=&command_args[i]-start;
                break;
            }
        }
        strncpy(data, start, count);
        data[count]='\0';
        if(command_args[i]=='|'){
            approve(command_args+i+1);
        }
    }
    else{
        printf("id: \n");
        scanf("%s", data);
    }
    FILE *f=fopen("approvals.bin", "rb+");
    user_info user;
    long long int fpp;
    while(1){
        fpp=ftell(f);
        fread(&user, sizeof(user_info), 1, f);
        if(strcmp(user.user_id, data)==0){
            if(strcmp(user.type, "student")==0){
                FILE *out=fopen("user list.bin", "ab");
                fwrite(&user, sizeof(user_info), 1, out);
                fclose(out);
            }
            else if(strcmp(user.type, "deact")==0){
                FILE *out=fopen("user list.bin", "rb+");
                user_info check;
                long long int fpp2;
                while(1){
                    fpp2=ftell(out);
                    fread(&check, sizeof(user_info), 1, out);
                    if(strcmp(check.user_id, data)==0){
                        fseek(out, fpp2, SEEK_SET);
                        sprintf(check.type, "%s", "student");
                        fwrite(&check, sizeof(user_info), 1, out);
                        fclose(out);
                        break;
                    }
                }
            }
            fseek(f, fpp, SEEK_SET);
            sprintf(user.type, "%s", "app");
            fwrite(&user, sizeof(user_info), 1, f);
            sprintf(output, "%s", "success");
            break;
        }
        if(feof(f)){
            sprintf(output, "%s", "not-found");
            break;
        }        
    }
    printf("%s\n", output);
    fclose(f);
    return;
}

void charge_account(char *command_args){
    if(is_student!=1){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    int amount;
    int pass_code;
    char data[3][20];
    if(is_read==1){
        put_data(command_args, 3, 20, data);
        sscanf(data[0], "%d", &amount);
        sscanf(data[2], "%d", &pass_code);
    }
    else{
        printf("amount: \n");
        scanf("%d", &amount);
        printf("card number: \n");
        scanf("%s", data[1]);
        printf("pass code: \n");
        scanf("%d", &pass_code);
    }
    if(amount<0){
        sprintf(output, "%s", "permission-denied");
    }
    else{
        current_user.credit+=amount;
        user_info user;
        FILE *f=fopen("user list.bin", "rb+");
        long long fpp;
        while(1){
            fpp=ftell(f);
            fread(&user, sizeof(user_info), 1, f);
            if(strcmp(user.user_id, current_user.user_id)==0){
                user=current_user;
                fseek(f, fpp, SEEK_SET);
                fwrite(&user, sizeof(user_info), 1, f);
                fclose(f);
                credit_change(amount, &current_day, current_time, "charge");
                sprintf(output, "%s", "success");
                break;
            }
        }
    }
    printf("%s\n", output);
    return;
}

void charge_student_account(char *command_args){
    if(is_student!=0){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    int amount;
    char data[2][15];
    if(is_read==1){
        put_data(command_args, 2, 15, data);
        sscanf(data[1], "%d", &amount);
    }
    else{
        printf("user id: \n");
        scanf("%s", data[0]);
        printf("amount: \n");
        scanf("%d", &amount);
    }
    FILE *f=fopen("user list.bin", "rb+");
    user_info user;
    long long fpp;
    while(1){
        fpp=ftell(f);
        fread(&user, sizeof(user_info), 1, f);
        if(strcmp(user.user_id, data[0])==0){
            user.credit+=amount;
            fseek(f, fpp, SEEK_SET);
            fwrite(&user, sizeof(user_info), 1, f);
            sprintf(output, "%s", "success");
            break;
        }
        if(feof(f)){
            sprintf(output, "%s", "not-found");
            break;
        }
    }
    printf("%s\n", output);
    fclose(f);
    return;
}

void send_charge(char *command_args){
    if(is_student!=1){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    int amount;
    char data[3][30];
    if(is_read==1){
        put_data(command_args, 3, 30, data);
        sscanf(data[1], "%d", &amount);
    }
    else{
        printf("user id: \n");
        scanf("%s", data[0]);
        printf("amount: \n");
        scanf("%d", &amount);
        printf("name: \n");
        scanf("%s", data[2]);
    }
    if(amount<0){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    printf("%d\n", current_user.credit);
    while(1){
        if(amount>current_user.credit){
            if(is_read==1){
                sprintf(output, "%s", "permission-denied");
                printf("%s\n", output);
                return;
            }
            else{
                getchar();
                printf("Not enough charge. Do you want to charge your account? y/n\n");
                char c;
                scanf("%c", &c);
                if(c=='y'){
                    charge_account(NULL);
                }
                else if(c=='n'){
                    sprintf(output, "%s", "permission-denied");
                    printf("%s\n", output);
                    return;
                }
                else{
                    printf("invalid input");
                }
            }
        }
        else{
            break;
        }
    }
    FILE *f=fopen("user list.bin", "rb+");
    user_info user;
    long long fpp;
    while(1){
        fpp=ftell(f);
        fread(&user, sizeof(user_info), 1, f);
        if(strcmp(user.user_id, data[0])==0){
            if(stricmp(user.name, data[2])==0){
                user.credit+=amount;
                fseek(f, fpp, SEEK_SET);
                fwrite(&user, sizeof(user_info), 1, f);
                break;
            }
            else{
                sprintf(output, "%s", "not-found");
                fclose(f);
                printf("%s\n", output);
                return;
            }
        }
        if(feof(f)){
            sprintf(output, "%s", "not-found");
            fclose(f);
            printf("%s\n", output);
            return;
        }
    }
    rewind(f);
    while(1){
        fpp=ftell(f);
        fread(&user, sizeof(user_info), 1, f);
        if(strcmp(user.user_id, current_user.user_id)==0){
            current_user.credit-=amount;
            user=current_user;
            fseek(f, fpp, SEEK_SET);
            fwrite(&user, sizeof(user_info), 1, f);
            credit_change(-1*amount, &current_day, current_time, "send-charge");
            sprintf(output, "%s", "success");
            break;
        }
    }
    fclose(f);
    printf("%s\n", output);
    return;
}

void print_meal(){
    FILE *m=fopen("meals.bin", "rb");
    meal_info meal;
    FILE *s=fopen("self.bin", "rb");
    self_info self;
    FILE *f=fopen("food.bin", "rb");
    food_info food;
    while(fread(&meal, sizeof(meal_info), 1, m)==1){
        rewind(s);
        while(fread(&self, sizeof(self_info), 1, s)==1){
            if(strcmp(meal.self_id, self.id)==0 &&
                (strcmp(self.type, "boyish")==0 && strcmp(current_user.gender, "male")==0 ||
                strcmp(self.type, "girlish")==0 && strcmp(current_user.gender, "female")==0)){
                
                if(day_difference(&meal.date, &current_day)>=3 && day_difference(&meal.date, &current_day)<=14){
                    if(meal.count>0){
                        rewind(f);
                        while(fread(&food, sizeof(food_info), 1, f)==1){
                            if(strcmp(meal.food_id, food.id)==0){
                                printf("self name: %s          self id: %s\n", self.name, self.id);
                                printf("date: %d-%d-%d          meal: %s\n", meal.date.year, meal.date.month, meal.date.day, meal.type);
                                printf("food name: %s          food id: %s\n\n", food.name, food.id);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    fclose(m);
    fclose(s);
    fclose(f);
    return;
}

void reserve(char *command_args){
    if(is_student!=1){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    reserve_info reserve;
    char data[4][15];
    if(is_read==1){
        put_data(command_args, 4, 15, data);
        strcpy(reserve.self_id, data[0]);
        sscanf(data[1], "%d-%d-%d", &reserve.date.year, &reserve.date.month, &reserve.date.day);
        strcpy(reserve.meal, data[2]);
        strcpy(reserve.food_id, data[3]);
    }
    else{
        printf("self id: \n");
        scanf("%s", reserve.self_id);
        printf("date: \n");
        scanf("%d-%d-%d", &reserve.date.year, &reserve.date.month, &reserve.date.day);
        printf("meal: \n");
        scanf("%s", reserve.meal);
        printf("food id: \n");
        scanf("%s", reserve.food_id);
    }
    if(day_difference(&reserve.date, &current_day)<3 || day_difference(&reserve.date, &current_day)>14){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    FILE *s=fopen("self.bin", "rb");
    self_info self;
    FILE *fo=fopen("food.bin", "rb");
    food_info food;
    while(1){
        fread(&self, sizeof(self_info), 1, s);
        if(strcmp(self.id, reserve.self_id)==0){
            fclose(s);
            break;
            
        }
        if(feof(s)){
            sprintf(output, "%s", "not-found");
            fclose(s);
            printf("%s\n", output);
            return;
        }
    }
    while(1){
        fread(&food, sizeof(food_info), 1, fo);
        if(strcmp(food.id, reserve.food_id)==0){
            fclose(fo);
            break;
        }
        if(feof(fo)){
            sprintf(output, "%s", "not-found");
            fclose(fo);
            printf("%s\n", output);
            return;    
        }
    }
    FILE *m=fopen("meals.bin", "rb+");
    meal_info meal;
    long long fpp;
    while(1){
        fpp=ftell(m);
        fread(&meal, sizeof(meal_info), 1, m);
        if(day_difference(&meal.date, &reserve.date)==0){
            if(strcmp(meal.self_id, reserve.self_id)==0){
                if(strcmp(meal.type, reserve.meal)==0){
                    if(strcmp(meal.food_id, reserve.food_id)==0){
                        if(strcmp(meal.gender, current_user.gender)==0){
                            if(meal.count>0){
                                while(1){
                                    if(current_user.credit<meal.food_price){
                                        if(is_read==1){
                                            sprintf(output, "%s", "permission-denied");
                                            fclose(m);
                                            printf("%s\n", output);
                                            return;
                                        }
                                        getchar();
                                        printf("Not enough charge. Do you want to charge your account? y/n\n");
                                        char c;
                                        scanf("%c", &c);
                                        if(c=='y'){
                                            charge_account(NULL);
                                        }
                                        else if(c=='n'){
                                            sprintf(output, "%s", "permission-denied");
                                            break;
                                        }
                                        else{
                                            printf("invalid");
                                        }
                                    }
                                    else{
                                        break;
                                    }
                                }
                                sprintf(reserve.user_id, "%s", current_user.user_id);
                                sprintf(reserve.status, "%s", "active");
                                FILE *out=fopen("reserves.bin", "rb+");
                                reserve_info reserve_temp;
                                while(1){
                                    fread(&reserve_temp, sizeof(reserve_info), 1, out);
                                    if( strcmp(reserve_temp.user_id, reserve.user_id)==0 &&
                                        strcmp(reserve_temp.meal, reserve.meal)==0 &&
                                        day_difference(&reserve_temp.date, &reserve.date)==0 &&
                                        strcmp(reserve_temp.status, "active")==0){
                                            
                                        fclose(out);
                                        sprintf(output, "%s", "permission-denied");
                                        printf("%s\n", output);
                                        fclose(m);
                                        return;
                                    }
                                    if(feof(out)){
                                        current_user.credit-=meal.food_price;
                                        meal.count--;
                                        reserve.start=meal.start;
                                        reserve.end=meal.end;
                                        reserve.food_price=meal.food_price;
                                        self_report(&reserve, 'r', NULL);
                                        fwrite(&reserve, sizeof(reserve_info), 1, out);
                                        fseek(m, fpp, SEEK_SET);
                                        fwrite(&meal, sizeof(meal_info), 1, m);
                                        fclose(out);
                                        credit_change(-1*meal.food_price, &current_day, current_time, "reserve");
                                        sprintf(output, "%s", "success");
                                        printf("%s\n", output);
                                        fclose(m);
                                        return;
                                    }
                                }
                            }
                            else{
                                sprintf(output, "%s", "permission-denied");
                                break;
                            }
                        }
                        else{
                            sprintf(output, "%s", "permission-denied");
                            break;
                        }
                    }
                }
            }
        }
        if(feof(m)){
            sprintf(output, "%s", "not-found");
            break;
        }
    }
    printf("%s\n", output);
    fclose(m);
    return;
}

void take_food(char *command_args){
    if(is_student!=1){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    day_info date;
    char data[3][15];
    if(is_read==1){
        put_data(command_args, 3, 15, data);
        sscanf(data[1], "%d-%d-%d", &date.year, &date.month, &date.day);
    }
    else{
        printf("self id:\n");
        scanf("%s", data[0]);
        printf("date:\n");
        scanf("%d-%d-%d", &date.year, &date.month, &date.day);
        printf("meal:\n");
        scanf("%s", data[2]);
    }
    FILE*f=fopen("self.bin", "rb");
    self_info self;
    while(1){
        fread(&self, sizeof(self_info), 1, f);
        if(strcmp(data[0], self.id)==0){
            break;
        }
        if(feof(f)){
            sprintf(output, "%s", "not-found");
            printf("%s\n", output);
            fclose(f);
            return;
        }
    }
    fclose(f);
    f=fopen("reserves.bin", "rb+");
    reserve_info reserve;
    long long fpp;
    while(1){
        fpp=ftell(f);
        fread(&reserve, sizeof(reserve_info), 1, f);
        if(day_difference(&date, &reserve.date)==0){
            if(strcmp(data[0], reserve.self_id)==0){
                if(strcmp(data[2], reserve.meal)==0){
                    if(strcmp(current_user.user_id, reserve.user_id)==0){
                        if(current_time>=reserve.start && current_time<=reserve.end){
                            if( strcmp(reserve.status, "active")==0 ||
                                strcmp(reserve.status, "daily")==0){
                                
                                self_report(&reserve, 't', NULL);
                                sprintf(reserve.status, "%s", "taken");
                                fseek(f, fpp, SEEK_SET);
                                fwrite(&reserve, sizeof(reserve_info), 1, f);
                                sprintf(output, "%s", "success");
                                break;
                            }
                            else{
                                sprintf(output, "%s", "permission-denied");
                                break;
                            }
                        }
                        else{
                            sprintf(output, "%s", "permission-denied");
                            break;
                        }
                    }
                    else{
                        sprintf(output, "%s", "permission-denied");
                        break;
                    }
                }
            }
        }
        if(feof(f)){
            sprintf(output, "%s", "permission-denied");
            break;
        }
    }
    printf("%s\n", output);
    fclose(f);
    return;
}

void cancel_reserve(char *command_args){
    if(is_student!=1){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    day_info date;
    char data[2][15];
    if(is_read==1){
        put_data(command_args, 2, 15, data);
        sscanf(data[0], "%d-%d-%d", &date.year, &date.month, &date.day);
    }
    else{
        printf("date:\n");
        scanf("%d-%d-%d", &date.year, &date.month, &date.day);
        printf("meal:\n");
        scanf("%s", data[1]);
    }
    FILE *f=fopen("reserves.bin", "rb+");
    reserve_info reserve;
    long long fpp;
    while(1){
        fpp=ftell(f);
        fread(&reserve, sizeof(reserve_info), 1, f);
        if(day_difference(&date, &reserve.date)==0){
            if(strcmp(current_user.user_id, reserve.user_id)==0){
                if(strcmp(data[1], reserve.meal)==0){
                    if(strcmp(reserve.status, "active")==0){
                        if( reserve.start-current_time>=100 &&
                            day_difference(&reserve.date, &current_day)==0 ||
                            day_difference(&reserve.date, &current_day)>=1){
                            
                            self_report(&reserve, 't', NULL);
                            sprintf(reserve.status, "%s", "cancel");
                            fseek(f, fpp, SEEK_SET);
                            fwrite(&reserve, sizeof(reserve_info), 1, f);
                            current_user.credit+=reserve.food_price/10*9;
                            user_info user;
                            FILE *out=fopen("user list.bin", "rb+");
                            while(1){
                                fpp=ftell(out);
                                fread(&user, sizeof(user_info), 1, out);
                                if(strcmp(user.user_id, current_user.user_id)==0){
                                    user=current_user;
                                    fseek(out, fpp, SEEK_SET);
                                    fwrite(&user, sizeof(user_info), 1, out);
                                    break;
                                }
                            }
                            fclose(out);
                            out=fopen("meals.bin", "rb+");
                            meal_info meal;
                            while(1){
                                fpp=ftell(out);
                                fread(&meal, sizeof(meal_info), 1, out);
                                if(day_difference(&reserve.date, &meal.date)==0){
                                    if(strcmp(meal.self_id, reserve.self_id)==0){
                                        if(strcmp(meal.type, reserve.meal)==0){
                                            meal.count++;
                                            fseek(out, fpp, SEEK_SET);
                                            fwrite(&meal, sizeof(meal_info), 1, out);
                                            break;
                                        }
                                    }
                                }
                            }
                            fclose(out);
                            credit_change(reserve.food_price/10*9, &current_day, current_time, "cancel");
                            sprintf(output, "%s", "success");
                            break;
                        }
                    }
                }
            }
        }
        if(feof(f)){
            sprintf(output, "%s", "not-found");
            break;
        }
    }
    printf("%s\n", output);
    fclose(f);
    return;
}

void daily_reserve(char *command_args){
    if(is_student!=1){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    reserve_info reserve;
    char data[2][15];
    if(is_read==1){
        put_data(command_args, 2, 15, data);
        strcpy(reserve.self_id, data[0]);
        strcpy(reserve.food_id, data[1]);
    }
    else{
        printf("self id: \n");
        scanf("%s", reserve.self_id);
        printf("food id: \n");
        scanf("%s", reserve.food_id);
    }
    FILE *s=fopen("self.bin", "rb");
    self_info self;
    FILE *fo=fopen("food.bin", "rb");
    food_info food;
    while(1){
        fread(&self, sizeof(self_info), 1, s);
        if(strcmp(self.id, reserve.self_id)==0){
            fclose(s);
            break;
            
        }
        if(feof(s)){
            sprintf(output, "%s", "not-found");
            fclose(s);
            printf("%s\n", output);
            return;
        }
    }
    while(1){
        fread(&food, sizeof(food_info), 1, fo);
        if(strcmp(food.id, reserve.food_id)==0){
            fclose(fo);
            break;
        }
        if(feof(fo)){
            sprintf(output, "%s", "not-found");
            fclose(fo);
            printf("%s\n", output);
            return;    
        }
    }
    FILE *m=fopen("meals.bin", "rb+");
    meal_info meal;
    long long fpp;
    while(1){
        fpp=ftell(m);
        fread(&meal, sizeof(meal_info), 1, m);
        if(day_difference(&current_day, &meal.date)==0){
            if(strcmp(meal.self_id, reserve.self_id)==0){
                if(strcmp(meal.food_id, reserve.food_id)==0){
                    if(strcmp(meal.gender, current_user.gender)==0){        
                        if(meal.count>=0){
                            if(meal.end-current_time>=70){
                                
                                while(1){
                                    if(current_user.credit<2*meal.food_price){
                                        if(is_read==1){
                                            sprintf(output, "%s", "permission-denied");
                                            printf("%s\n", output);
                                            fclose(m);
                                            return;
                                        }
                                        else{
                                            getchar();
                                            printf("Not enough charge. Do you want to charge your account? y/n\n");
                                            char c;
                                            scanf("%c", &c);
                                            if(c=='y'){
                                                charge_account(NULL);
                                            }
                                            else{
                                                sprintf(output, "%s", "permission-denied");
                                                printf("%s\n", output);
                                                fclose(m);
                                                return;
                                            }
                                        }
                                    }
                                    else{
                                        break;
                                    }
                                }
                                sprintf(reserve.user_id, "%s", current_user.user_id);
                                sprintf(reserve.status, "%s", "daily");
                                FILE *out=fopen("reserves.bin", "rb+");
                                reserve_info reserve_temp;
                                while(1){
                                    fread(&reserve_temp, sizeof(reserve_info), 1, out);
                                    if( strcmp(reserve_temp.user_id, reserve.user_id)==0 &&
                                        strcmp(reserve_temp.meal, reserve.meal)==0 &&
                                        day_difference(&reserve_temp.date, &reserve.date)==0 &&
                                        strcmp(reserve_temp.status, "cancel")!=0){
                                            
                                        fclose(out);
                                        sprintf(output, "%s", "permission-denied");
                                        printf("%s\n", output);
                                        fclose(m);
                                        return;
                                    }
                                    if(feof(out)){
                                        current_user.credit-=2*meal.food_price;
                                        meal.count--;
                                        reserve.start=meal.start;
                                        reserve.end=meal.end;
                                        reserve.food_price=meal.food_price;
                                        reserve.date=meal.date;
                                        self_report(&reserve, 'r', NULL);
                                        sprintf(reserve.meal, "%s", meal.type);
                                        fwrite(&reserve, sizeof(reserve_info), 1, out);
                                        fseek(m, fpp, SEEK_SET);
                                        fwrite(&meal, sizeof(meal_info), 1, m);
                                        fclose(out);
                                        credit_change(-2*meal.food_price, &current_day, current_time, "daily-reserve");
                                        sprintf(output, "%s", "success");
                                        printf("%s\n", output);
                                        fclose(m);
                                        return;
                                    }
                                }
                            }
                            else{
                                sprintf(output, "%s", "permission-denied");
                                printf("%s\n", output);
                                fclose(m);
                                return;
                            }
                        }
                    }
                }
            }
        }
        if(feof(m)){
            sprintf(output, "%s", "permission-denied");
            break;
        }
    }
    printf("%s\n", output);
    fclose(m);
    return;
}

void change_self(char *command_args){
    if(is_student!=1){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    reserve_info reserve;
    char data[3][15];
    if(is_read==1){
        put_data(command_args, 3, 15, data);
        sscanf(data[0], "%d-%d-%d", &reserve.date.year, &reserve.date.month, &reserve.date.day);
        strcpy(reserve.meal, data[1]);
        strcpy(reserve.self_id, data[2]);
    }
    else{
        printf("date: \n");
        scanf("%d-%d-%d", &reserve.date.year, &reserve.date.month, &reserve.date.day);
        printf("meal: \n");
        scanf("%s", reserve.meal);
        printf("new self id: \n");
        scanf("%s", reserve.self_id);
    }
    FILE *s=fopen("self.bin", "rb");
    self_info self;
    while(1){
        fread(&self, sizeof(self_info), 1, s);
        if(strcmp(self.id, reserve.self_id)==0){
            fclose(s);
            break;
        }
        if(feof(s)){
            sprintf(output, "%s", "not-found");
            fclose(s);
            printf("%s\n", output);
            return;
        }
    }
    FILE *m=fopen("reserves.bin", "rb+");
    reserve_info reserve_temp;
    long long fpp;
    while(1){
        fpp=ftell(m);
        fread(&reserve_temp, sizeof(reserve_info), 1, m);
        if(day_difference(&reserve.date, &reserve_temp.date)==0){
            if(strcmp(reserve.meal, reserve_temp.meal)==0){
                if(strcmp(current_user.user_id, reserve_temp.user_id)==0){
                    if( reserve_temp.start-current_time>=300 && 
                        day_difference(&current_day, &reserve_temp.date)==0 ||
                        day_difference(&reserve_temp.date, &current_day)>=1){
                        
                        self_report(&reserve_temp, 'c', reserve.self_id);
                        sprintf(reserve_temp.self_id, "%s", reserve.self_id);
                        fseek(m, fpp, SEEK_SET);
                        fwrite(&reserve_temp, sizeof(reserve_info), 1, m);
                        FILE *out=fopen("meals.bin", "rb+");
                        meal_info meal;
                        while(1){
                            fpp=ftell(out);
                            fread(&meal, sizeof(meal_info), 1, out);
                            if(day_difference(&reserve_temp.date, &meal.date)==0){
                                if(strcmp(meal.self_id, reserve_temp.self_id)==0){
                                    if(strcmp(meal.type, reserve_temp.meal)==0){
                                        meal.count--;
                                        fseek(out, fpp, SEEK_SET);
                                        fwrite(&meal, sizeof(meal_info), 1, out);
                                        break;
                                    }
                                }
                            }
                        }
                        rewind(out);
                        while(1){
                            fpp=ftell(out);
                            fread(&meal, sizeof(meal_info), 1, out);
                            if(day_difference(&reserve_temp.date, &meal.date)==0){
                                if(strcmp(meal.self_id, reserve.self_id)==0){
                                    if(strcmp(meal.type, reserve_temp.meal)==0){
                                        meal.count++;
                                        fseek(out, fpp, SEEK_SET);
                                        fwrite(&meal, sizeof(meal_info), 1, out);
                                        break;
                                    }
                                }
                            }
                        }
                        fclose(out);
                        sprintf(output, "%s", "success");
                        break;
                    }
                    else{
                        sprintf(output, "%s", "permission-denied");
                        break;
                    }
                }
            }
        }
        if(feof(m)){
            sprintf(output, "%s", "not-found");
            break;
        }
    }
    printf("%s\n", output);
    fclose(m);
    return;
}

void print_daily_meals(){
    FILE *m=fopen("meals.bin", "rb");
    meal_info meal;
    FILE *s=fopen("self.bin", "rb");
    self_info self;
    FILE *f=fopen("food.bin", "rb");
    food_info food;
    while(fread(&meal, sizeof(meal_info), 1, m)==1){
        rewind(s);
        while(fread(&self, sizeof(self_info), 1, s)==1){
            if(strcmp(meal.self_id, self.id)==0 &&
                (strcmp(self.type, "boyish")==0 && strcmp(current_user.gender, "male")==0 ||
                strcmp(self.type, "girlish")==0 && strcmp(current_user.gender, "female")==0)){
                
                if(day_difference(&meal.date, &current_day)==0){
                    if(meal.count>0){
                        rewind(f);
                        while(fread(&food, sizeof(food_info), 1, f)==1){
                            if(strcmp(meal.food_id, food.id)==0){
                                printf("self name: %s          self id: %s\n", self.name, self.id);
                                printf("date: %d-%d-%d          meal: %s\n", meal.date.year, meal.date.month, meal.date.day, meal.type);
                                printf("food name: %s          food id: %s\n\n", food.name, food.id);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    fclose(m);
    fclose(s);
    fclose(f);
    return;
}

void add_news(char *command_args){
    if(is_student!=0){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    news_info news;
    char data[3][500];
    if(is_read==1){
        put_data(command_args, 3, 500, data);
        sscanf(data[0], "%[^\n]s", news.title);
        sscanf(data[1], "%[^\n]s", news.content);
        sscanf(data[2], "%d-%d-%d", &news.end.year, &news.end.month, &news.end.day);
    }
    else{
        getchar();
        printf("news title:\n");
        scanf("%[^\n]s", news.title);
        getchar();
        printf("news content:\n");
        scanf("%[^\n]s", news.content);
        printf("end date:\n");
        scanf("%d-%d-%d", &news.end.year, &news.end.month, &news.end.day);
    }
    FILE *f=fopen("news.bin", "ab");
    fwrite(&news, sizeof(news_info), 1, f);
    sprintf(output, "%s", "success");
    printf("%s\n", output);
    fclose(f);
    return;
}

void check_news(){
    if(is_student!=1){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    FILE *f=fopen("news.bin", "rb");
    news_info news;
    while(fread(&news, sizeof(news_info), 1, f)==1){
        if(day_difference(&news.end, &current_day)>=1){
            printf("%s\n%s\n", news.title, news.content);
            sprintf(output, "%s", "success");
        }
        else{
            sprintf(output, "%s", "not-found");
        }
    }
    printf("%s\n", output);
    fclose(f);
    return;
}

void print_approval(){
    FILE *f=fopen("approvals.bin", "rb");
    user_info user;
    while(fread(&user, sizeof(user_info), 1, f)==1){
        if(strcmp(user.type, "app")!=0){
            printf("name: %s          family name: %s\n", user.name, user.family);
            printf("user id: %s          national id code: %s\n", user.user_id, user.national_id_code);
            printf("birthdate: %s          gender: %s\n\n", user.birthdate, user.gender);
        }
    }
    fclose(f);
    return;
}

void print_reserves(){
    FILE *f=fopen("reserves.bin", "rb");
    reserve_info reserve;
    while(fread(&reserve, sizeof(reserve_info), 1, f)==1){
        if(strcmp(current_user.user_id, reserve.user_id)==0){
            if(day_difference(&current_day, &reserve.date)>=-7 && day_difference(&current_day, &reserve.date)<=7){
                FILE *s=fopen("self.bin", "rb");
                self_info self;
                char self_name[30];
                FILE *fo=fopen("food.bin", "rb");
                food_info food;
                char food_name[30];
                while(1){
                    fread(&self, sizeof(self_info), 1, s);
                    if(strcmp(self.id, reserve.self_id)==0){
                        strcpy(self_name, self.name);
                        break;
                    }
                }
                while(1){
                    fread(&food, sizeof(food_info), 1, fo);
                    if(strcmp(food.id, reserve.food_id)==0){
                        strcpy(food_name, food.name);
                        break;
                    }
                }
                fclose(s);
                fclose(fo);
                printf("self name: %s          date: %d-%d-%d\n", self_name, reserve.date.year, reserve.date.month, reserve.date.day);
                printf("meal: %s          food name: %s\n", reserve.meal, food_name);
                printf("status: %s\n\n", reserve.status);
            }
        }
    }
    fclose(f);
    return;
}

void print_self_report(){
    FILE *f=fopen("self report.bin", "rb");
    self_report_info self;
    while(fread(&self, sizeof(self_report_info), 1, f)==1){
        printf("self id: %s          food id: %s\n", self.self_id, self.food_id);
        printf("meal: %s          date: %d-%d-%d\n", self.meal, self.date.year, self.date.month, self.date.day);
        printf("reserves: %d          taken: %d\n\n", self.reserve_count, self.taken_count);
    }
    fclose(f);
    return;
}

void print_credit_report(){
    FILE *f=fopen("credit report.bin", "rb");
    credit_info cred;
    while(fread(&cred, sizeof(credit_info), 1, f)==1){
        if(strcmp(cred.user_id, current_user.user_id)==0){
            printf("amount: %d          operation: %s\n", cred.amount, cred.type);
            printf("time: %d          date: %d-%d-%d\n", cred.time, cred.date.year, cred.date.month, cred.date.day);
        }
    }
    fclose(f);
    return;
}

void add_agent(char *command_args){
    if(is_student!=1){
        sprintf(output, "%s", "permission-denied");
        printf("%s\n", output);
        return;
    }
    day_info date;
    char meal[15];
    char new_user[15];
    char data[3][15];
    if(is_read==1){
        put_data(command_args, 3, 15, data);
        sscanf(data[0], "%d-%d-%d", &date.year, &date.month, &date.day);
        strcpy(meal, data[1]);
        strcpy(new_user, data[2]);
    }
    else{
        printf("date: \n");
        scanf("%d-%d-%d", &date.year, &date.month, &date.day);
        printf("meal: \n");
        scanf("%s", meal);
        printf("new user id: \n");
        scanf("%s", new_user);
    }
    FILE *m=fopen("user list.bin", "rb");
    user_info user;
    while(1){
        fread(&user, sizeof(user_info), 1, m);
        if(strcmp(new_user, user.user_id)==0){
            fclose(m);
            break;
        }
        if(feof(m)){
            sprintf(output, "%s", "not-found");
            fclose(m);
            printf("%s\n", output);
            return;
        }
    }
    m=fopen("reserves.bin", "rb+");
    reserve_info reserve;
    long long fpp;
    while(1){
        fpp=ftell(m);
        fread(&reserve, sizeof(reserve_info), 1, m);
        if(day_difference(&reserve.date, &date)==0){
            if(strcmp(meal, reserve.meal)==0){
                if(strcmp(current_user.user_id, reserve.user_id)==0){
                    sprintf(reserve.user_id, "%s", new_user);
                    fseek(m, fpp, SEEK_SET);
                    fwrite(&reserve, sizeof(reserve_info), 1, m);
                    sprintf(output, "%s", "success");
                    break;
                }
            }
        }
        if(feof(m)){
            sprintf(output, "%s", "not-found");
            break;
        }
    }
    printf("%s\n", output);
    fclose(m);
    return;
}

void admin_panel(){
    char choice[10];
    while(1){
        printf("current date: %d-%d-%d\ncurrent time: \n\n", current_day.year, current_day.month, current_day.day, current_time);
        printf("a:logout    b:change pass    c:change student pass\nd:add user    e:approve user    f:add self\ng:add food    h:add meal    i:remove user\nj:deactivate user    k:charge student account    l:add news\nm:set time    n:show approvals    o:show self report\n");
        scanf("%s", choice);
        if(choice[0]=='a'){
            logout(NULL);
            if(is_student==-1){
                return;
            }
        } 
        else if(choice[0]=='b'){
            change_password(NULL);
        }
        else if(choice[0]=='c'){
            change_student_pass(NULL);
        }
        else if(choice[0]=='d'){
            add_user(NULL);
        }
        else if(choice[0]=='e'){
            approve(NULL);
        }
        else if(choice[0]=='f'){
            add_self(NULL);
        }
        else if(choice[0]=='g'){
            add_food(NULL);
        }
        else if(choice[0]=='h'){
            add_meal(NULL);
        }
        else if(choice[0]=='i'){
            remove_user(NULL);
        }
        else if(choice[0]=='j'){
            deactivate_user(NULL);
        }
        else if(choice[0]=='k'){
            charge_student_account(NULL);
        }
        else if(choice[0]=='l'){
            add_news(NULL);
        }
        else if(choice[0]=='m'){
            set_time(NULL);
        }
        else if(choice[0]=='n'){
            print_approval();
        }
        else if(choice[0]=='o'){
            print_self_report();
        }
        else{
            printf("Invalid input\n");
        }
    }
}

void student_panel(){
    char choice[10];
    while(1){
        printf("current date: %d-%d-%d\ncurrent time: \n\n", current_day.year, current_day.month, current_day.day, current_time);
        printf("credit: %d", current_user.credit);
        printf("a:logout    b:change pass    d:reserve\ne:charge account    f:send charge    g:take food\nh:show news\ni:set time    j:cancel reserve    k:daily reserve\nl:change self    m:show meals    n:show reserves\no:show daily meals    p:food report    q:financial report    r:add agent\n");
        scanf("%s", choice);
        if(choice[0]=='a'){
            logout(NULL);
            if(is_student==-1){
                return;
            }
        } 
        else if(choice[0]=='b'){
            change_password(NULL);
        }
        else if(choice[0]=='d'){
            reserve(NULL);
        }
        else if(choice[0]=='e'){
            charge_account(NULL);
        }
        else if(choice[0]=='f'){
            send_charge(NULL);
        }
        else if(choice[0]=='g'){
            take_food(NULL);
        }
        else if(choice[0]=='h'){
            check_news();
        }
        else if(choice[0]=='i'){
            set_time(NULL);
        }
        else if(choice[0]=='j'){
            cancel_reserve(NULL);
        }
        else if(choice[0]=='k'){
            daily_reserve(NULL);
        }
        else if(choice[0]=='l'){
            change_self(NULL);
        }
        else if(choice[0]=='m'){
            print_meal();
        }
        else if(choice[0]=='n'){
            print_reserves();
        }
        else if(choice[0]=='o'){
            print_daily_meals();
        }
        else if(choice[0]=='p'){
            print_food_report();
        }
        else if(choice[0]=='q'){
            print_credit_report();
        }
        else if(choice[0]=='r'){
            add_agent(NULL);
        }
        else{
            printf("Invalid input\n");
        }
    }
}

void run_command(FILE *out, char *command){
    char command_id[5];
    char command_name[50];
    char command_args[500];
    char *p1, *p2=NULL;
    int i;
    for(i=0; command[i]!='\0'; i++){
        if(command[i]=='#'){
            p1=&command[i];
            command[i]='\0';
            break;
        }
    }
    for(i++; command[i]!='\0'; i++){
        if(command[i]=='#'){
            p2=&command[i];
            command[i]='\0';
            break;
        }
    }
    strcpy(command_id, command);
    strcpy(command_name, ++p1);
    if(p2!=NULL){
        strcpy(command_args, ++p2);
    }
    
    if(strcmp(command_name, "login")==0){
        login(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "logout")==0){
        logout(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "change-pass")==0){
        change_password(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "approve")==0){
        approve(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "change-student-pass")==0){
        change_student_pass(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "remove-student")==0){
        remove_user(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "deactivate")==0){
        deactivate_user(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "register")==0){
        add_user(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "define-self")==0){
        add_self(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "define-food")==0){
        add_food(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "define-meal-plan")==0){
        add_meal(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "charge-student-account")==0){
        charge_student_account(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "add-news")==0){
        add_news(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "reserve")==0){
        reserve(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "take-food")==0){
        take_food(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "charge-account")==0){
        charge_account(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "send-charge")==0){
        send_charge(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "cancel-reserve")==0){
        cancel_reserve(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "daily-reserve")==0){
        daily_reserve(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "define-agent")==0){
        add_agent(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "change-self")==0){
        change_self(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "check-news")==0){
        check_news();
        fprintf(out, "%s#%s\n", command_id, output);
    }
    else if(strcmp(command_name, "change-datetime")==0){
        set_time(command_args);
        fprintf(out, "%s#%s\n", command_id, output);
    }
    return;
}

void receive_command(){
    char address[100];
    getchar();
    printf("enter address:\n");
    scanf("%[^\n]s", address);
    FILE *inp=fopen(address, "r");
    FILE *out=fopen("40131039.txt", "a");
    char command[500];
    while(fgets(command, 500, inp)!=NULL){
        run_command(out, command);
    }
    fclose(inp);
    fclose(out);
    is_read=0;
    return;
}

int main(){
    create_files();
    printf("Welcome\n");
    char choice[10];
    while(1){
        printf("current date: %d-%d-%d\ncurrent time: \n\n", current_day.year, current_day.month, current_day.day, current_time);
        printf("a:Login    b:exit    c:set time\nd:register    f:read from file\n");
        scanf("%s", choice);
        if(choice[0]=='a'){
            login(NULL);
        }
        else if(choice[0]=='b'){
            exit(0);
        }
        else if(choice[0]=='c'){
            set_time(NULL);
        }
        else if(choice[0]=='d'){
            add_user(NULL);
        }
        else if(choice[0]=='f'){
            is_read=1;
            receive_command();
        }
        else{
            printf("invalid input\n");
        }
        if(is_student==0){
            admin_panel();
        } 
        else if(is_student==1){
            student_panel();
        }
    }
    return 0;
}