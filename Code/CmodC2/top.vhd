library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity top is
	Port (
		Clk   : in  STD_LOGIC;
		Q1A   : in  STD_LOGIC;
		Q1B   : in  STD_LOGIC;
		Q1Z   : in  STD_LOGIC;
		Q2A   : in  STD_LOGIC;
		Q2B   : in  STD_LOGIC;
		Q2Z   : in  STD_LOGIC;
		Q3A   : in  STD_LOGIC;
		Q3B   : in  STD_LOGIC;
		Q3Z   : in  STD_LOGIC;
		LE    : in  STD_LOGIC;
		SCLK  : in  STD_LOGIC;
		MISO  : out STD_LOGIC;
		Seek  : in  STD_LOGIC
	);
end top;

architecture Behavioral of top is
signal Count1 : std_logic_vector (3 downto 0);
signal Count2 : std_logic_vector (3 downto 0);
signal Count3 : std_logic_vector (3 downto 0);
signal Data   : std_logic_vector (15 downto 0);
signal Found1 : std_logic;
signal Found2 : std_logic;
signal Found3 : std_logic;
signal Clear : std_logic;
begin

	-- 3x Quadrature Counters
	QuadDecoder1: entity QuadratureDecoder port map (Clk, Q1A, Q1B, Q1Z, Count1, Seek, Found1, Clear);
	QuadDecoder2: entity QuadratureDecoder port map (Clk, Q2A, Q2B, Q2Z, Count2, Seek, Found2, Clear);
	QuadDecoder3: entity QuadratureDecoder port map (Clk, Q3A, Q3B, Q3Z, Count3, Seek, Found3, Clear);


	-- SPI communication
	Wire: entity Wire port map (Clk, LE, SCLK, MISO, Data, Clear);

	-- Combinatorial logic
	Data <= '0' & Found1 & Found2 & Found3 & Count1 & Count2 & Count3;

end Behavioral;