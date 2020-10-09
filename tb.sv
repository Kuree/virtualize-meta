module top;

import cgra_info_pkg::*;

CGRAInfo info;
byte input_data[];
byte output_data[];
bitstream_t bitstream;
bitstream_entry_t bs_entry;

initial begin
    info = new("vectors/design.meta");

    assert(info.input_size == 8);
    assert(info.output_size == 4);

    assert(info.input_filenames.size() == 2);
    assert(info.output_filenames.size() == 3);
    assert(info.input_filenames[0] == "vectors/a");

    input_data = info.get_input_data(0);
    assert(input_data[0] == 'h78);
    assert(input_data[input_data.size() - 1] == 'ha);
    assert(input_data.size() == 8);

    output_data = info.get_output_data(0);
    assert(output_data[0] == 'h78);
    assert(output_data[output_data.size() - 1] == 'ha);
    assert(output_data.size() == 4);

    // get bitstream
    bitstream = info.get_bitstream();
    assert(bitstream.size() == 11);
    bs_entry = bitstream[1];
    assert(bs_entry.addr == 'h01000203);
    assert(bs_entry.data == 'h48000200);

end

endmodule