`ifndef CGRA_INFO_PKG
`define CGRA_INFO_PKG

package cgra_info_pkg;

import "DPI-C" function chandle parse_placement(string filename);
import "DPI-C" function chandle parse_metadata(string filename);

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

typedef struct packed {
    int x;
    int y;
} Position;

class CGRAInfo;
    string bitstream_filename;
    string input_filenames[];
    string output_filenames[];

    int input_size;
    int output_size;
    int num_groups;

    Position input_pos[];
    Position output_pos[];

    Position reset_pos;

    function new(string meta_filename);
        chandle meta_info, io_info;
        int num_inputs, num_outputs;
        string placement_filename;

        meta_info = parse_metadata(meta_filename);
        assert_(meta_info != null, $sformatf("Unable to find %s", meta_filename));
        placement_filename = get_placement_filename(meta_info);
        bitstream_filename = get_bitstream_filename(meta_info);
        input_size = get_input_size(meta_info);
        output_size = get_output_size(meta_info);

        io_info = parse_placement(placement_filename);
        assert_(io_info != null, $sformatf("Unable to find %s", placement_filename));

        num_groups = get_num_groups(io_info);
        num_inputs = get_num_inputs(io_info);
        num_outputs = get_num_outputs(io_info);

        input_filenames = new[num_inputs];
        input_pos = new[num_inputs];
        output_filenames = new[num_outputs];
        output_pos = new[num_outputs];

        for (int i = 0; i < num_inputs; i++) begin
            int x, y;
            input_filenames[i] = get_input_filename(meta_info, i);
            x = get_input_x(io_info, i);
            y = get_input_y(io_info, i);
            input_pos[i] = {x, y};
        end

        for (int i = 0; i < num_outputs; i++) begin
            int x, y;
            output_filenames[i] = get_output_filename(meta_info, i);
            x = get_output_x(io_info, i);
            y = get_output_y(io_info, i);
            output_pos[i] = {x, y};
        end
    endfunction

    static function void assert_(bit cond, string msg);
        assert (cond) else begin
            $display("%s", msg);
            $stacktrace;
            $finish(1);
        end
    endfunction
    

endclass


endpackage


`endif // CGRA_INFO_PKG