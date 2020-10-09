#ifndef VIRTUALIZE_META_LIBRARY_H
#define VIRTUALIZE_META_LIBRARY_H

#define MAX_NUM_IO 32
#define BUFFER_SIZE 1024
#define MAX_NUM_PARSER 16

struct KernelInfo {
    int num_groups;
    int num_inputs;
    int num_outputs;
    // x y x y x y ...
    int inputs[MAX_NUM_IO * 2];
    int outputs[MAX_NUM_IO * 2];

    // index to the inputs, need to multiply by 2
    int reset_port;
};

struct MetaInfo {
    char placement_filename[BUFFER_SIZE];
    char bitstream_filename[BUFFER_SIZE];

    char input_filenames[MAX_NUM_IO][BUFFER_SIZE];
    char output_filenames[MAX_NUM_IO][BUFFER_SIZE];

    int input_size;
    int output_size;
};

void *parse_placement(char *filename);
void *parse_metadata(char *filename);

// helper functions to access data from SV
int get_num_groups(void *info);
int get_num_inputs(void *info);
int get_num_outputs(void *info);
int get_input_x(void *info, int index);
int get_input_y(void *info, int index);
int get_output_x(void *info, int index);
int get_output_y(void *info, int index);
int get_reset_index(void *info);

char *get_placement_filename(void *info);
char *get_bitstream_filename(void *info);
char *get_input_filename(void *info, int index);
char *get_output_filename(void *info, int index);
int get_input_size(void *info);
int get_output_size(void *info);

#endif//VIRTUALIZE_META_LIBRARY_H
