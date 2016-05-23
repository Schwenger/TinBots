ENTITY FlipFlop IS
	PORT (reset, set : IN Bit; q : INOUT Bit; nq : INOUT Bit := '1');
END;

ARCHITECTURE structural of FlipFlop IS
	BEGIN
		PROCESS (reset, set, q, nq) BEGIN
			q <= reset nor nq;
			nq <= set nor q;
		END PROCESS;
	END structural;


ENTITY Register1 IS
	PORT (in0 : IN Bit; out0 : OUT Bit; en, clk : IN Bit);
END;

ARCHITECTURE structural of Register1 IS
	COMPONENT FlipFlop
		PORT (reset, set : IN Bit; q : INOUT Bit; nq : INOUT Bit := '1');
	END COMPONENT;
	
	SIGNAL reset, set, q, nq : Bit;
	
	BEGIN
		ff: FlipFlop PORT MAP (reset, set, q, nq);
	
		PROCESS (clk) BEGIN
			out0 <= q;
			reset <= en and (not in0);
			set <= en and in0;
		END PROCESS;
	END structural;
		
		

ENTITY Register4 IS
	PORT (in3, in2, in1, in0 : IN Bit; out3, out2, out1, out0 : OUT Bit; en, clk : IN Bit);
END Register4;

ARCHITECTURE structural of Register4 IS
	COMPONENT Register1
		PORT (in0 : IN Bit; out0 : OUT Bit; en, clk : IN Bit);
	END COMPONENT;

	BEGIN
		reg0: Register1 PORT MAP (in0, out0, en, clk);
		reg1: Register1 PORT MAP (in1, out1, en, clk);
		reg2: Register1 PORT MAP (in2, out2, en, clk);
		reg3: Register1 PORT MAP (in3, out3, en, clk);
	END structural;


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


ENTITY FFTest IS
END;

ARCHITECTURE test OF FFTest IS
	COMPONENT FlipFlop
		PORT (reset, set : IN Bit; q, nq : INOUT Bit);
	END COMPONENT;
	
	SIGNAL reset, set, q, nq : Bit;
	
	BEGIN
		ff: FlipFlop PORT MAP (reset, set, q, nq);
		
		PROCESS BEGIN
			wait;
		END PROCESS;
	END test;
