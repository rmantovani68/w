/*
* File DBF.H
* Definizioni relative alla struttura di file DBF
*/
#ifndef _dbf_
#define _dbf_


typedef struct {
        char dbf_id;
        char last_update[3];
        long last_rec;
        unsigned short data_offset;
        unsigned short rec_size;
        char filler[20];
} DBF_HEAD;

typedef struct {
        char field_name[11];
        char field_type;
        char dummy[4];
        union {
                unsigned short char_len;
                struct {
                        char len;
                        char dec;
                } num_size;
        } len_info;
        char filler[14];
} FIELD_REC;

         
#endif        
