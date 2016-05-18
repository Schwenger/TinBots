entity mystery is
Port ( a,b : in Bit;
       clk : in Bit;
       x,z : out Bit);
end mystery;

architecture Behavioral of mystery is
signal rev : Bit := '1';
begin

process(clk)
begin
        if (clk'event) then
                x <= (a and b) or (not rev);
                rev <= not rev;
                z <= (a or b) after 10 ns;
        end if;
end process;

end Behavioral;

------------------------------------------------------------------------

entity gen_in is
Port (a, b, clk : out Bit);
end gen_in;

architecture behavior of gen_in is
begin

process begin
-- 0 ns
wait for 5 ns;
-- 5 ns
clk <= '1';
wait for 5 ns;
-- 10 ns
clk <= '0';
b <= '1';
wait for 5 ns;
-- 15 ns
clk <= '1';
wait for 5 ns;
-- 20 ns
clk <= '0';
a <= '1';
b <= '0';
wait for 5 ns;
-- 25 ns
clk <= '1';
wait for 5 ns;
-- 30 ns
clk <= '0';
b <= '1';
wait for 5 ns;
-- 35 ns
clk <= '1';
wait for 5 ns;
-- 40 ns
clk <= '0';
a <= '0';
b <= '0';
wait for 5 ns;
-- 45 ns
clk <= '1';
wait for 5 ns;
-- 50 ns
clk <= '0';
wait for 5 ns;
-- 55 ns
clk <= '1';
wait for 5 ns;
-- 60 ns
clk <= '0';
wait for 5 ns;
end process;

end behavior;

------------------------------------------------------------------------

entity testbench is
end testbench;

architecture structure of testbench is
component gen_in
  port (a, b, clk : out Bit);
end component;
component mystery
  port (a, b, clk : in Bit;
        x, z : out Bit);
end component;
signal a, b, clk, x, z : BIT;
begin

i0: gen_in port map (a, b, clk);
i1: mystery port map (a, b, clk, x, z);

end structure;
