`ifndef CGRA_INFO_HH
`define CGRA_INFO_HH

package cgra_info;

import "DPI-C" function chandle parse_placement(string);
import "DPI-C" function chandle parse_metadata(string);
import "DPI-C" function int get_num_groups(chandle info);
import "DPI-C" function int get_num_inputs(chandle info);
import "DPI-C" function int get_num_outputs(chandle info);
import "DPI-C" function int get_input_x(chandle info, int index);
import "DPI-C" function int get_input_y(chandle info, int index);
import "DPI-C" function int get_output_x(chandle info, int index);
import "DPI-C" function int get_output_y(chandle info, int index);
import "DPI-C" function int get_reset_index(chandle info);

import "DPI-C" function string get_placement_filename(chandle info);
import "DPI-C" function string get_bitstream_filename(chandle info);
import "DPI-C" function string get_input_filename(chandle info, int index);
import "DPI-C" function string get_output_filename(chandle info, int index);
import "DPI-C" function int get_input_size(chandle info);
import "DPI-C" function int get_output_size(chandle info);


typedef struct {
    int x;
    int y;
} Position;


class CGRAInfo;
    string bitstream_filename;
    string input_filenames[];
    string output_filenames[];

    int input_size;
    int output_size;

    Position input_pos[];
    Position output_pos[];

    int reset_index;

    function new(string meta_filename);
        chandle meta_info io_info;
        int num_input, num_output;
        string placement_filename;

        meta_info = parse_metadata(meta_filename);
        placement_filename = get_placement_filename(meta_info);
        bitstream_filename = get_bitstream_filename(meta_info);

        io_info = parse_placement(placement_filename);

        num_input = get_num_inputs(io_info);
        num_output = get_num_outputs(io_info);

        input_filenames = new[num_input];
        output_filenames = new[num_output];

        for (int i = 0; i < num_input; i++) begin
            input_filenames[i] = get_input_filename(meta_info, i);
        end

        for (int i = 0; i < num_output; i++) begin
            output_filenames[i] = get_output_filename(meta_info, i);
        end

        input_size = get_input_size(meta_info);
        output_size = get_input_size(meta_info);

        // get coordinates
        input_pos = new[num_input];
        output_pos = new[num_output];

        for (int i = 0; i < num_input; i++) begin
            int x, y;
            x = get_input_x(io_info, i);
            y = get_input_y(io_info, i);
            input_pos[i] = {x, y};
        end

        for (int i = 0; i < num_output; i++) begin
            int x, y;
            x = get_output_x(io_info, i);
            y = get_output_y(io_info, i);
            output_pos[i] = {x, y};
        end

    endfunction

endclass

endpackage

`endif // CGRA_INFO_HH