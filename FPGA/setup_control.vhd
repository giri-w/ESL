--
	-- @file setup_control.vhd
	-- @brief Toplevel file template file which can be used as a reference for implementing gpmc communication.
	-- @author Jan Jaap Kempenaar, University of Twente 2014
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity setup_control is
  generic(
	  ENCODER_WIDTH          : integer := 12;
    DATA_WIDTH             : integer := 16;
    GPMC_ADDR_WIDTH_HIGH   : integer := 10;
    GPMC_ADDR_WIDTH_LOW    : integer := 1;
    -- RAM_SIZE should be a power of 2
    RAM_SIZE               : integer := 32
	);
  port (
    CLOCK_50               : in    std_logic;
		
    -- GPMC side
    GPMC_DATA              : inout std_logic_vector(DATA_WIDTH - 1 downto 0);
    GPMC_ADDR              : in    std_logic_vector(GPMC_ADDR_WIDTH_HIGH downto GPMC_ADDR_WIDTH_LOW);
    GPMC_nPWE              : in    std_logic;
    GPMC_nOE               : in    std_logic;
    GPMC_FPGA_IRQ          : in    std_logic;
    GPMC_nCS6              : in    std_logic;
    GPMC_CLK               : in    std_logic;
		
    -- FPGA side
		-- Output headers
    F_IN                   : in    std_logic_vector(15 downto 0);
    F_OUT                  : out   std_logic_vector(15 downto 0);
    
    -- Pulse width modulators
    PWM1A                  : out   std_logic;
    PWM1B                  : out   std_logic;
    PWM1C                  : out   std_logic;
    
    PWM2A                  : out   std_logic;
    PWM2B                  : out   std_logic;
    PWM2C                  : out   std_logic;
    
    PWM3A                  : out   std_logic;
    PWM3B                  : out   std_logic;
    PWM3C                  : out   std_logic;
    
    PWM4A                  : out   std_logic;
    PWM4B                  : out   std_logic;
    PWM4C                  : out   std_logic;
		
    -- Encoders
    ENC1A                  : in    std_logic;
    ENC1B                  : in    std_logic;
    ENC1I                  : in    std_logic;
    
    ENC2A                  : in    std_logic;
    ENC2B                  : in    std_logic;
    ENC2I                  : in    std_logic;
    
    ENC3A                  : in    std_logic;
    ENC3B                  : in    std_logic;
    ENC3I                  : in    std_logic;
    
    ENC4A                  : in    std_logic;
    ENC4B                  : in    std_logic;
    ENC4I                  : in    std_logic
	);
end setup_control;


architecture structure of setup_control is
  -- GPMC controller component for FPGA
  component ramstix_gpmc_driver is
    generic(
      DATA_WIDTH           : integer := 16;
			ENCODER_WIDTH        : integer := 12;
      GPMC_ADDR_WIDTH_HIGH : integer := 10;
      GPMC_ADDR_WIDTH_LOW  : integer := 1;
      RAM_SIZE             : integer := 32
		);
    port(
      clk                  : in    std_logic;
			
      -- Data from FPGA to Overo Fire (input for GPMC driver)
      reg0_in              : in    std_logic_vector(DATA_WIDTH - 1 downto 0);
      reg1_in              : in    std_logic_vector(DATA_WIDTH - 1 downto 0);
      reg2_in              : in    std_logic_vector(DATA_WIDTH - 1 downto 0);
      reg3_in              : in    std_logic_vector(DATA_WIDTH - 1 downto 0);
			
      -- Data from Overo Fire to FPGA (output of GPMC driver)
      reg4_out             : out   std_logic_vector(DATA_WIDTH - 1 downto 0);
      reg5_out             : out   std_logic_vector(DATA_WIDTH - 1 downto 0);
      reg6_out             : out   std_logic_vector(DATA_WIDTH - 1 downto 0);
      reg7_out             : out   std_logic_vector(DATA_WIDTH - 1 downto 0);
			
      GPMC_DATA            : inout std_logic_vector(DATA_WIDTH - 1 downto 0);
      GPMC_ADDR            : in    std_logic_vector(GPMC_ADDR_WIDTH_HIGH downto GPMC_ADDR_WIDTH_LOW);
      GPMC_nPWE            : in    std_logic;
      GPMC_nOE             : in    std_logic;
      GPMC_FPGA_IRQ        : in    std_logic;
      GPMC_nCS6            : in    std_logic;
      GPMC_CLK             : in    std_logic
		);
		
	end component;
	
  -- INSERT YOUR COMPONENTS HERE
  component QuadratureEncoder
  	generic (
			ENCODER_WIDTH        : natural := 12
		);
		port(
			clk 				         : IN   STD_LOGIC;
			signalA1		         : IN   STD_LOGIC;
			signalB1		         : IN   STD_LOGIC;
			signalA2		         : IN   STD_LOGIC;
			signalB2		         : IN   STD_LOGIC;
			c_mtrPan             : OUT  STD_LOGIC_VECTOR(ENCODER_WIDTH-1 downto 0);
			c_mtrTilt            : OUT  STD_LOGIC_VECTOR(ENCODER_WIDTH-1 downto 0)
		);
	end component QuadratureEncoder;
	
	component PulseWidthModulator
		port(
			clk                  : in  STD_LOGIC;
			duty_cycle1 	       : in  INTEGER RANGE 0 TO 255;  
			dir1					       : in  STD_LOGIC_VECTOR(1 downto 0); 
			duty_cycle2 	       : in  INTEGER RANGE 0 TO 255;  
			dir2					       : in  STD_LOGIC_VECTOR(1 downto 0); 
			signA1				       : out STD_LOGIC;
			signB1				       : out STD_LOGIC;
			signPWM1    	       : out STD_LOGIC;
			signA2				       : out STD_LOGIC;
			signB2				       : out STD_LOGIC;
			signPWM2   	         : out STD_LOGIC
		);
	end component PulseWidthModulator;
	
	-- Define signals to connect the component to the gpmc_driver
	signal msb_buffer_out    : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal lsb_buffer_out    : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal msb_buffer_out2   : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal lsb_buffer_out2   : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal msb_buffer_in     : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal lsb_buffer_in     : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal msb_buffer_in2    : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal lsb_buffer_in2    : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal duty_mtrPan       : integer range 0 to 255;
	signal dir_mtrPan 	  	 : std_logic_vector(1 downto 0);
	signal duty_mtrTilt      : integer range 0 to 255;
	signal dir_mtrTilt 			 : std_logic_vector(1 downto 0);
	signal enc_cntPan        : std_logic_vector(ENCODER_WIDTH-1 downto 0);
	signal enc_cntTilt       : std_logic_vector(ENCODER_WIDTH-1 downto 0);
	
	begin
  -- Map GPMC controller to I/O.
  gpmc_driver              : ramstix_gpmc_driver generic map(
		DATA_WIDTH              => DATA_WIDTH,
		GPMC_ADDR_WIDTH_HIGH    => GPMC_ADDR_WIDTH_HIGH,
		GPMC_ADDR_WIDTH_LOW     => GPMC_ADDR_WIDTH_LOW,
		RAM_SIZE                => RAM_SIZE
	)
	port map (
		clk                     => CLOCK_50,
		-- Normal start from reg0_in to reg3_in and reg4_out to reg7_out
		-- Linux offset        : idx 0
		reg0_in                 => lsb_buffer_in,       -- LSB
		reg1_in                 => msb_buffer_in,       -- MSB
		
		-- Linux offset        : idx 1
		reg2_in                 => lsb_buffer_in2,      -- LSB
		reg3_in                 => msb_buffer_in2,      -- MSB
		
		-- Linux offset        : idx 2
		reg4_out                => lsb_buffer_out,      -- LSB
		reg5_out                => msb_buffer_out,      -- MSB
		
		-- Linux offset        : idx 3
		reg6_out                => lsb_buffer_out2,     -- LSB
		reg7_out                => msb_buffer_out2,     -- MSB
		
		GPMC_DATA               => GPMC_DATA,
		GPMC_ADDR               => GPMC_ADDR,
		GPMC_nPWE               => GPMC_nPWE,
		GPMC_nOE                => GPMC_nOE,
		GPMC_FPGA_IRQ           => GPMC_FPGA_IRQ,
		GPMC_nCS6               => GPMC_nCS6,
		GPMC_CLK                => GPMC_CLK
	);
	
  enc                      : component QuadratureEncoder 
  generic map(
		ENCODER_WIDTH           => ENCODER_WIDTH;
	)
  port map (
		clk   		              => CLOCK_50,
	  signalA1		            => ENC1A,
	  signalB1		            => ENC1B,
	  signalA2		            => ENC2A,
	  signalB2		            => ENC2B,
	  c_mtrPan   	            => enc_cntPan,
		c_mtrTilt  	            => enc_cntTilt  
	);
  
  pwm                      : component PulseWidthModulator port map (
		clk                     => CLOCK_50,
		duty_cycle1             => duty_mtrPan,  
		dir1			              => dir_mtrPan, 
	  duty_cycle2             => duty_mtrTilt, 
		dir2			              => dir_mtrTilt,
	  signA1		              => PWM1A,
		signB1		              => PWM1B,
		signPWM1   	            => PWM1C,
	  signA2		              => PWM2A,
	  signB2		              => PWM2B,
		signPWM2   	            => PWM2C
		
	);
	
	-- Signal testing : 1. Same with receive code, 2. Equal to 65535
  --msb_buffer_in                <= duty_mtrPan & duty_mtrTilt;
	--lsb_buffer_in                <= "000000000001 " & dir_mtrPan & dir_mtrTilt;
	--msb_buffer_in                <= (others  => '0');
	--lsb_buffer_in                <= (others  => '1');
	
	
  -- Encoder Counter for Motor PAN 
  --msb_buffer_in                <= duty_mtrPan & duty_mtrTilt;
	--lsb_buffer_in                <= duty_mtrPan & duty_mtrTilt;
	--msb_buffer_in                <= (others  => '1');
	--lsb_buffer_in                <= (others  => '1');
	msb_buffer_in                <= (others  => enc_cntPan(11));
	lsb_buffer_in(15 downto 12)  <= enc_cntPan(11);
	lsb_buffer_in(11 downto  0)  <= enc_cntPan;
  -- Encoder Counter for Motor TILT
	--msb_buffer_in2               <= (others => '0');
	--msb_buffer_in2               <= (others => '1');
	msb_buffer_in2               <= (others => enc_cntTilt(11));
	lsb_buffer_in2(15 downto 12) <= enc_cntTilt(11);
	lsb_buffer_in2(11 downto  0) <= enc_cntTilt;
  
  -- Signal for Motor
  duty_mtrPan		               <= to_integer(unsigned(msb_buffer_out(15  downto 8)));
  duty_mtrTilt	               <= to_integer(unsigned(msb_buffer_out(7   downto 0)));
  dir_mtrPan		               <= lsb_buffer_out(3  downto 2);
  dir_mtrTilt		               <= lsb_buffer_out(1  downto 0);
	
end architecture;
