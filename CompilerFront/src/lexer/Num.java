package lexer;

public class Num extends Token {
	//整数加上标签
	public final int value;
	public Num(int v) { super(Tag.NUM); value = v; }
	public String toString() { return "" + value; }
}
