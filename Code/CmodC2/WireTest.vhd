--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   14:22:56 07/13/2016
-- Design Name:   
-- Module Name:   D:/Sync/ExternalProjects/CNC_QuaDdECODER/QuadraDec3x/WireTest.vhd
-- Project Name:  QuadraDec3x
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: Wire
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
 
ENTITY WireTest IS
END WireTest;
 
ARCHITECTURE behavior OF WireTest IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT Wire
    PORT (
       clk : IN  std_logic;
       latch : IN  std_logic;
       sclk : IN  std_logic;
       miso : OUT  std_logic;
       data : IN  std_logic_vector(14 downto 0)
    );
    END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal latch : std_logic := '0';
   signal sclk : std_logic := '0';
   signal data : std_logic_vector(14 downto 0) := (others => '0');

 	--Outputs
   signal miso : std_logic;

   -- Clock period definitions
   constant clk_period : time := 62.5 ns;
   constant sclk_period : time := 10 ns;

   constant Clk_period : time := 62.5 ns;
	 constant Quad_period : time := 10 us;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: Wire PORT MAP (
      clk   => clk,
      latch => latch,
      sclk  => sclk,
      miso  => miso,
      data  => data
   );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 
   sclk_process :process
   begin
		sclk <= '0';
		wait for sclk_period/2;
		sclk <= '1';
		wait for sclk_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for clk_period*10;

      -- insert stimulus here 

      wait;
   end process;

END;
