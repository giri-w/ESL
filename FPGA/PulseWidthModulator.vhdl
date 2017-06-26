library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity PulseWidthModulator is

	
    Port ( clk     	    : in  STD_LOGIC;
		       
		       duty_cycle1 	: in  INTEGER RANGE 0 TO 255;  
		       dir1 				: in  STD_LOGIC_VECTOR(1 downto 0); 
					 duty_cycle2 	: in  INTEGER RANGE 0 TO 255;  
		       dir2			 		: in  STD_LOGIC_VECTOR(1 downto 0); 
					 signA1				: out STD_LOGIC;
					 signB1				: out STD_LOGIC;
		       signPWM1    	: out STD_LOGIC;
					 signA2				: out STD_LOGIC;
					 signB2				: out STD_LOGIC;
		       signPWM2    	: out STD_LOGIC
					);
   end PulseWidthModulator;

architecture Behavioral of PulseWidthModulator is
      constant period:integer:=255;
	  
      signal pwm_reg1,pwm_next1:std_logic;
      signal counter1,counter_next1 :integer:=0;
	  signal pwm_reg2,pwm_next2:std_logic;
      signal counter2,counter_next2 :integer:=0;
      
	  
	  
    begin
    --register
         process(clk,dir1,dir2)
              begin
                   if clk='1' and clk'event then
				   
				   CASE dir1 IS
						when "00" 	=> signA1 <= '0'; signB1 <= '0'; pwm_reg1	<= '0'; -- '0'
						when "10" 	=> signA1 <= '1'; signB1 <= '0'; pwm_reg1	<= pwm_next1;  -- '0'
						when "01" 	=> signA1 <= '1'; signB1 <= '1'; pwm_reg1	<= '0'; -- '0'
						when "11" 	=> signA1 <= '0'; signB1 <= '1'; pwm_reg1	<= pwm_next1; -- '0'
						when others => signA1 <= '0'; signB1 <= '0'; pwm_reg1	<= '0';-- nothing
				   END CASE;
				   
				   CASE dir2 IS
						when "00" 	=> signA2 <= '0'; signB2 <= '0'; pwm_reg2	<= '0'; -- '0'
						when "10" 	=> signA2 <= '1'; signB2 <= '0'; pwm_reg2	<= pwm_next2;  -- '0'
						when "01" 	=> signA2 <= '1'; signB2 <= '1'; pwm_reg2	<= '0'; -- '0'
						when "11" 	=> signA2 <= '0'; signB2 <= '1'; pwm_reg2	<= pwm_next2; -- '0'
						when others => signA2 <= '0'; signB2 <= '0'; pwm_reg2	<= '0';-- nothing
				   END CASE;
				   
				   counter1<=counter_next1;
				   counter2<=counter_next2;

                         
                   end if;
          end process;
-- Motor 1 Tilting
counter_next1	<= 0    when counter1 = period else counter1+1;
pwm_next1		<= '1'  when counter1 < duty_cycle1 else '0'; 

-- Motor 2 Panning
counter_next2	<= 0   when counter2 = period else counter2+1;
pwm_next2		<= '1' when counter2 < duty_cycle2 else '0'; 

		  
--Buffer
signPWM1<=pwm_reg1;  
signPWM2<=pwm_reg2;    
   
end Behavioral;
