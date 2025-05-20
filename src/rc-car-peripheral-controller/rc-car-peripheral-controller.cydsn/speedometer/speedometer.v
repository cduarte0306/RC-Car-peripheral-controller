
//`#start header` -- edit after this line, do not edit this line
// ========================================
//
// Copyright YOUR COMPANY, THE YEAR
// All Rights Reserved
// UNPUBLISHED, LICENSED SOFTWARE.
//
// CONFIDENTIAL AND PROPRIETARY INFORMATION
// WHICH IS THE PROPERTY OF your company.
//
// ========================================
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 04/29/2025 at 19:57
// Component: speed_measurement_core
module speedometer (
    output [15:0] counts,
    input start,
    input clock,
    input enc_pulse
);

    reg [15:0] r_counts;
    reg [15:0] r_counts_buff;
    reg prev_enc;

    assign counts = r_counts;

    wire rising_edge = enc_pulse & ~prev_enc;

    always @(posedge clock) begin
        if (!start) begin
            r_counts      <= 16'b0;
            r_counts_buff <= 16'b0;
            prev_enc      <= 1'b0;
        end else begin
            prev_enc <= enc_pulse;

            if (rising_edge) begin
                r_counts      <= r_counts_buff;
                r_counts_buff <= 0;
            end else begin
                r_counts_buff <= r_counts_buff + 1;
            end
        end
    end
endmodule

//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line
