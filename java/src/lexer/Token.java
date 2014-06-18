package lexer;
//Token 就是这个的标签
public class Token {

	public final int tag;
	public Token(int t) { tag = t; }
	public String toString() {return "" + (char)tag;}
}
