library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.NUMERIC_STD.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity Wire is
    Port ( 
			Clk  : in   STD_LOGIC;
			
			LE   : in   STD_LOGIC;
	      SCLK : in   STD_LOGIC;
         MISO : out  STD_LOGIC;
			Q    : in   STD_LOGIC_VECTOR(15 downto 0);
			
			Latch : out STD_LOGIC

	 );
end Wire;

architecture Behavioral of Wire is

signal rising_SCLK : STD_LOGIC;
signal rising_LE : STD_LOGIC;

signal rQ : STD_LOGIC_VECTOR(15 downto 0);
signal rSCLK : STD_LOGIC;
signal rLE : STD_LOGIC;

begin

	-- combinatorial logic
	rising_SCLK <= (rSCLK xor SCLK) and SCLK;
	rising_LE <= (rLE xor LE) and LE;

	-- sequential logic
	process(Clk)
	begin
		if rising_edge(Clk) then
			rSCLK <= SCLK;
			rLE <= LE;
			Latch <= rising_LE;
			if rising_LE = '1' then
				rQ <= Q;
			end if;
			if rising_SCLK = '1' then
				MISO <= rQ(15);
				rQ <= rQ(14 downto 0) & '0';
			end if;
		end if;
	end process;
	
end Behavioral;
