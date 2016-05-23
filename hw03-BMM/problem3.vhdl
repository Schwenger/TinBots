ENTITY Lot IS
	PORT (a, b : IN Bit; cars : OUT Integer);
END Lot;

ARCHITECTURE behavioral of Lot IS
	SIGNAL c, d : Bit;
	
	BEGIN
		PROCESS (a, b) BEGIN
			c <= a or b;
			d <= not (a or b);
		END PROCESS;
		
		PROCESS
			VARIABLE counter : Integer := 0;
		BEGIN
			cars <= counter;
			
			WAIT ON c;
			
			IF (a = '1') THEN
				counter := counter + 1;
			ELSE
				counter := counter - 1;
			END IF;
			
			IF (counter < 0) THEN
				counter := 0;
			END IF;
			
			WAIT ON d;
		END PROCESS;
	END behavioral;


ENTITY Test IS
END Test;

ARCHITECTURE test of Test IS
	COMPONENT Lot
		PORT (a, b : IN Bit; cars : OUT Integer);
	END COMPONENT;
	
	SIGNAL a, b : Bit;
	SIGNAL cars : Integer;
	
	BEGIN
		l : Lot PORT MAP (a, b, cars);
		
		PROCESS BEGIN
			a <= '1';
			wait for 5 ns;
			b <= '1';
			wait for 5 ns;
			a <= '0';
			wait for 5 ns;
			b <= '0';
			wait for 5 ns;
			
			a <= '1';
			wait for 5 ns;
			b <= '1';
			wait for 5 ns;
			a <= '0';
			wait for 5 ns;
			b <= '0';
			wait for 5 ns;
			
			a <= '1';
			wait for 5 ns;
			b <= '1';
			wait for 5 ns;
			a <= '0';
			wait for 5 ns;
			b <= '0';
			wait for 5 ns;
			
			b <= '1';
			wait for 5 ns;
			a <= '1';
			wait for 5 ns;
			b <= '0';
			wait for 5 ns;
			a <= '0';
			wait for 5 ns;
			
			wait;
		END PROCESS;
	END test;
		
