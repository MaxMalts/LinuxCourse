#ifndef PHONEBOOK_H
#define PHONEBOOK_H

typedef struct {
    char name[100];
    char surname[100];
    int age;
    char phone[100];
} user_data;


extern long get_phonebook_user(const char* surname, unsigned int len, user_data* output_data);
extern long add_phonebook_user(user_data* input_data);
extern long del_phonebook_user(const char* surname, unsigned int len);

#endif