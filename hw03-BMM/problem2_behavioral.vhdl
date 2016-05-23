ENTITY Register4 IS
	PORT (in3, in2, in1, in0 : IN Bit; out3, out2, out1, out0 : OUT Bit; en, clk : IN Bit);
END Register4;

ARCHITECTURE behavioral of Register4 IS
	BEGIN
		PROCESS (clk)
			VARIABLE t3 : Bit := '0';
			VARIABLE t2 : Bit := '0';
			VARIABLE t1 : Bit := '0';
			VARIABLE t0 : Bit := '0';
		BEGIN
			out3 <= t3;
			out2 <= t2;
			out1 <= t1;
			out0 <= t0;
			IF (en = '1') THEN
				t3 := in3;
				t2 := in2;
				t1 := in1;
				t0 := in0;
			END IF;
		END PROCESS;
	END behavioral;


ENTITY Test IS
END;

ARCHITECTURE test of Test IS
	COMPONENT Register4
		PORT (in3, in2, in1, in0 : IN Bit; out3, out2, out1, out0 : OUT Bit; en, clk : IN Bit);
	END COMPONENT;
	
	SIGNAL in3, in2, in1, in0 : Bit;
	SIGNAL out3, out2, out1, out0 : Bit;
	SIGNAL en, clk : Bit;
	
	BEGIN
		reg: Register4 PORT MAP (in3, in2, in1, in0, out3, out2, out1, out0, en, clk);
		
		PROCESS BEGIN
			clk <= '1';
			wait for 5 ns;
			clk <= '0';
			wait for 5 ns;
			in3 <= '1';
			in1 <= '1';
			en <= '1';
			clk <= '1';
			wait for 5 ns;
			clk <= '0';
			en <= '0';
			in3 <= '0';
			in1 <= '0';
			wait for 5 ns;
			clk <= '1';
			wait for 5 ns;
			clk <= '0';
			wait for 5 ns;
			in3 <= '0';
			in2 <= '1';
			in1 <= '0';
			in0 <= '1';
			en <= '1';
			clk <= '1';
			wait for 5 ns;
			clk <= '0';
			en <= '0';
			in2 <= '0';
			in0 <= '0';
			wait for 5 ns;
			clk <= '1';
			wait for 5 ns;
			wait;
		END PROCESS;
	END test;
