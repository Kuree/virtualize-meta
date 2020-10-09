module top;

import cgra_info_pkg::*;

CGRAInfo info;


initial begin
    info = new("vectors/design.meta");

    assert(info.input_size == 8);
    assert(info.output_size == 4);

    assert(info.input_filenames.size() == 2);
    assert(info.output_filenames.size() == 3);
    assert(info.input_filenames[0] == "vectors/a");

end

endmodule