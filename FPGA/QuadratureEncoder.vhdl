LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY QuadratureEncoder IS
	GENERIC (
		DATA_WIDTH : natural := 10
	);
    PORT ( clk 				: IN   STD_LOGIC;
	         signalA1		: IN   STD_LOGIC;
		  		 signalB1		: IN   STD_LOGIC;
		  		 signalA2		: IN   STD_LOGIC;
		  		 signalB2		: IN   STD_LOGIC;
		  		 c_output1  : OUT  STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0);
		  		 c_output2  : OUT  STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0)
	);
		   
END QuadratureEncoder;

ARCHITECTURE bhv OF QuadratureEncoder IS
	SIGNAL   P0, P1, Q0, Q1, U1,D1,U2,D2    : STD_LOGIC;
  SIGNAL   count1,count2				          : SIGNED (DATA_WIDTH-1 downto 0) := (others => '0');

BEGIN
	p_encoder : PROCESS (clk)
	BEGIN
		IF (clk'EVENT and clk='1') THEN
			-- Counter of Encoder Rotation
			c_output1 <= std_logic_vector(count1);
			c_output2 <= std_logic_vector(count2);
			
			-- Determine the direction of the encoder
			Q0 <= signalA1 xor signalB1;
			Q1 <= signalB1;
			
			U1 <= (not signalA1 and not signalB1 and Q1) or (not signalA1 and Q1 and not Q0) or (signalA1 and not Q1 and not Q0) or (signalA1 and signalB1 and not Q1);
			D1 <= (not signalA1 and signalB1 and not Q1) or (not signalA1 and not Q1 and Q0) or (signalA1 and Q1 and Q0) or (signalA1 and not signalB1 and Q1);
			
			P0 <= signalA2 xor signalB2;
			P1 <= signalB2;
			
			U2 <= (not signalA2 and not signalB2 and P1) or (not signalA2 and P1 and not P0) or (signalA2 and not P1 and not P0) or (signalA2 and signalB2 and not P1);
			D2 <= (not signalA2 and signalB2 and not P1) or (not signalA2 and not P1 and P0) or (signalA2 and P1 and P0) or (signalA2 and not signalB2 and P1);
			
			IF (U1 = '1') THEN
				count1 <= count1 + 1;
			END IF;
			
			IF (D1 = '1') THEN
				count1 <= count1 - 1;
			END IF;
			
			IF (U2 = '1') THEN
				count2 <= count2 + 1;
			END IF;
			
			IF (D2 = '1') THEN
				count2 <= count2 - 1;
			END IF;
			
			
		END IF;
	END PROCESS;

END bhv;
