--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   13:20:38 07/13/2016
-- Design Name:   
-- Module Name:   D:/Sync/ExternalProjects/CNC_QuaDdECODER/QuadraDec3x/QuadTest.vhd
-- Project Name:  QuadraDec3x
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: QuadratureDecoder
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY QuadTest IS
END QuadTest;
 
ARCHITECTURE behavior OF QuadTest IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT QuadratureDecoder
    PORT(
         Clk : IN  std_logic;
         Seek : IN  std_logic;
         QuadA : IN  std_logic;
         QuadB : IN  std_logic;
         QuadZ : IN  std_logic;
         Position : OUT  std_logic_vector(7 downto 0)
    );
    END COMPONENT;
    

   --Inputs
   signal Clk : std_logic := '0';
   signal Seek : std_logic := '0';
   signal QuadA : std_logic := '0';
   signal QuadB : std_logic := '0';
   signal QuadZ : std_logic := '0';

 	--Outputs
   signal Position : std_logic_vector(7 downto 0);

   -- Clock period definitions
   constant Clk_period : time := 62.5 ns;
	 constant Quad_period : time := 10 us;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: QuadratureDecoder PORT MAP (
          Clk => Clk,
          Seek => Seek,
          QuadA => QuadA,
          QuadB => QuadB,
          QuadZ => QuadZ,
          Position => Position
        );

   -- Clock process definitions
   Clk_process :process
   begin
		Clk <= '0';
		wait for Clk_period/2;
		Clk <= '1';
		wait for Clk_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for Clk_period*10;

      -- insert stimulus here 

		for I in 0 to 3 loop
			QuadA <= '1';
			wait for Quad_period;
			QuadB <= '1';
			wait for Quad_period;
			QuadA <= '0';
			wait for Quad_period;
			QuadB <= '0';
			wait for Quad_period;
		end loop;

		Seek <= '1';

		QuadB <= '1';
		wait for Quad_period / 2;
		QuadZ <= '1';
		wait for Quad_period / 2;
		QuadA <= '1'; 
		wait for Quad_period;
		QuadB <= '0';
		wait for Quad_period;
		QuadA <= '0';
		wait for Quad_period / 2;
		QuadZ <= '1';
		wait for Quad_period / 2;
		
		
		for I in 0 to 2 loop
			QuadB <= '1';
			wait for Quad_period;
			QuadA <= '1';
			wait for Quad_period;
			QuadB <= '0';
			wait for Quad_period;
			QuadA <= '0';
			wait for Quad_period;
		end loop;

		QuadA <= '1';

      wait;
   end process;

END;
