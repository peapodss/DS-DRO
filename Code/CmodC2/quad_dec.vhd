library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.NUMERIC_STD.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity QuadratureDecoder is
	Port (
		Clk   : in  STD_LOGIC;
		QuadA : in  STD_LOGIC;
      QuadB : in  STD_LOGIC;
      QuadZ : in  STD_LOGIC;
		Count : out  STD_LOGIC_VECTOR (3 downto 0);
		Seek  : in  STD_LOGIC;
		Found : out STD_LOGIC;
		Clear : in STD_LOGIC
	);
end QuadratureDecoder;

architecture Behavioral of QuadratureDecoder is

	-- registers
	signal rQuadA : STD_LOGIC_VECTOR(1 downto 0);
	signal rQuadB : STD_LOGIC_VECTOR(1 downto 0);
	signal rQuadZ : STD_LOGIC;
	signal rCount : STD_LOGIC_VECTOR(3 downto 0) := (OTHERS => '0');
	signal rFound : STD_LOGIC;

	-- combinatorials
	signal Step  : STD_LOGIC;
	signal Dir   : STD_LOGIC;
	signal Reset : STD_LOGIC;
	
	
begin 

	-- combinatorial logic
	Dir <= rQuadA(0) xor rQuadB(1);
	Step <= Dir xor rQuadB(0) xor rQuadA(1);
	Count <= rCount;
	Reset <= (rQuadZ xor QuadZ) and QuadZ and Seek and not rFound;
	Found <= rFound;
	
	-- sequential logic
	process(Clk)
	begin
		if rising_edge(Clk) then
			rQuadA <= rQuadA(0) & QuadA;
			rQuadB <= rQuadB(0) & QuadB;
			rQuadZ <= QuadZ;
			if Reset = '1' then
				rCount <= (OTHERS => '0');
				rFound <= '1';
			else
				if Clear = '1' then
					rFound <= '0';
				end if;
				if Step = '1' then
					if Dir = '1' then
						rCount <= rCount + '1';
					else
						rCount <= rCount - '1';
					end if;
				end if;
			end if;
		end if;
	end process;

end Behavioral;