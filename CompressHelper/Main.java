import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Reader;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public final class Main {
	
	protected static final class HuffmanNode {
		
		public final char character;
		
		public String code;
		
		public final int count;
		
		public final HuffmanNode left;
		public final HuffmanNode right;
		
		public HuffmanNode(char ch, int cnt) {
			this.character = ch;
			this.count = cnt;
			this.left = this.right = null;
		}
		
		public HuffmanNode(HuffmanNode l, HuffmanNode r) {
			this.character = (char)0;
			this.left = l;
			this.right = r;
			this.count = l.count + r.count;
		}
		
		public void serialize(Writer wtr) throws IOException {
			wtr.write(this.left.character);
			wtr.write(this.right.character);
			if (this.left.character == (char)0)
				this.left.serialize(wtr);
			if (this.right.character == (char)0)
				this.right.serialize(wtr);
		}
		
		public void setCode(String str) {
			if (str == null)
				str = "";
			this.code = str;
			if (this.left != null)
				this.left.setCode(str + "0");
			if (this.right != null)
				this.right.setCode(str + "1");
		}
		
	}
	
	protected static void doHuffman(Reader rdr, Writer wtr, OutputStream out) throws IOException {
		StringBuffer buf = new StringBuffer();
		int[] stat = new int[0x10000];
		{
			int ch;
			while ((ch = rdr.read()) != -1) {
				buf.append((char)ch);
				stat[ch]++;
			}
		}
		List<HuffmanNode> li = new ArrayList<>();
		HuffmanNode tree;
		{
			for (int i = 0; i < stat.length; i++) {
				if (stat[i] > 0)
					li.add(new HuffmanNode((char)i, stat[i]));
			}
			List<HuffmanNode> nli = new ArrayList<>(li);
			Collections.sort(nli, new Comparator<HuffmanNode>() {
				public int compare(HuffmanNode a, HuffmanNode b) {
					return a.count - b.count;
				}
			});
			while (nli.size() > 1) {
				HuffmanNode nr = nli.remove(0), nl = nli.remove(0), np = new HuffmanNode(nl, nr);
				int i = 0, s = nli.size();
				while (i < s && np.count > nli.get(i).count) {
					i++;
				}
				nli.add(i, np);
			}
			tree = nli.get(0);
			tree.setCode(null);
		}
		String[] map = new String[0x10000];
		for (HuffmanNode n : li) {
			map[n.character] = n.code;
		}
		tree.serialize(wtr);
		wtr.flush();
		int len = buf.length();
		out.write(new byte[] {(byte)len, (byte)(len >> 8), (byte)(len >> 16), (byte)(len >> 24)});
		int b = 0, cnt = 0;
		for (int i = 0, blen = buf.length(); i < blen; i++) {
			String seq = map[buf.charAt(i)];
			for (int j = 0, slen = seq.length(); j < slen; j++) {
				b |= (seq.charAt(j) == '1' ? 1 : 0) << cnt++;
				if (cnt == 8) {
					out.write(b);
					b = 0;
					cnt = 0;
				}
			}
		}
		if (cnt > 0)
			out.write(b);
	}
	
	protected static final int FORWARD_LENGTH = 1 << 10;
	protected static final int MIN_MATCH_LENGTH = 3;
	protected static final int MAX_MATCH_LENGTH = (1 << 6) + 2;
	
	protected static void doLZSS(InputStream in, OutputStream out) throws IOException {
		byte[] buf = new byte[0x1000], obuf = new byte[1 + 8 * 2];
		int cur = 0, ed = 0, ocnt = 0, oed = 1;
		boolean fin = false;
		while (!fin || cur < ed) {
			if (cur + FORWARD_LENGTH > buf.length) {
				System.arraycopy(buf, cur - FORWARD_LENGTH, buf, 0, FORWARD_LENGTH + ed - cur);
				ed = FORWARD_LENGTH + ed - cur;
				cur = FORWARD_LENGTH;
			}
			while (!fin && ed - cur < MAX_MATCH_LENGTH) {
				int ch = in.read();
				if (ch != -1) {
					buf[ed++] = (byte)ch;
				} else {
					fin = true;
				}
			}
			int mpos = 0, mlen = 0;
			for (int st = Math.max(cur - FORWARD_LENGTH, 0); st < cur; st++) {
				if (buf[st] != buf[cur])
					continue;
				int len = 1;
				while (buf[st + len] == buf[cur + len] && cur + len < ed) {
					len++;
				}
				if (len > mlen) {
					mpos = st;
					mlen = len;
					if (mlen >= MAX_MATCH_LENGTH)
						break;
				}
			}
			if (mlen >= MIN_MATCH_LENGTH) {
				int tmp = mpos - (cur - FORWARD_LENGTH);
				tmp |= (Math.min(mlen, MAX_MATCH_LENGTH) - MIN_MATCH_LENGTH) << 10;
				obuf[0] |= 1 << ocnt++;
				obuf[oed++] = (byte)tmp;
				obuf[oed++] = (byte)(tmp >> 8);
				cur += Math.min(mlen, MAX_MATCH_LENGTH);
			} else {
				ocnt++;
				obuf[oed++] = buf[cur++];
			}
			if (ocnt == 8) {
				out.write(obuf, 0, oed);
				Arrays.fill(obuf, (byte)0);
				ocnt = 0;
				oed = 1;
			}
		}
		if (ocnt > 0)
			out.write(obuf, 0, oed);
	}
	
	public static void main(String[] args) throws IOException {
		if (args.length != 3 && args.length != 4)
			throw new IllegalArgumentException();
		File fin = new File(args.length == 4 ? args[2] : args[1]),
			fout = new File(args.length == 4 ? args[3] : args[2]);
		InputStream in = new BufferedInputStream(new FileInputStream(fin));
		OutputStream out = new BufferedOutputStream(new FileOutputStream(fout));
		Reader rdr = new InputStreamReader(in, args.length == 4 ? args[1] : "UTF-8");
		switch (args[0].toUpperCase()) {
		case "H":
		case "HUFFMAN":
			doHuffman(rdr, new OutputStreamWriter(out, "UTF-16LE"), out);
			break;
		case "SH":
		case "HUFFMAN-S":
			Writer wtr = new OutputStreamWriter(new FileOutputStream(new File("tree.dat")), "UTF-16LE");
			doHuffman(rdr, wtr, out);
			break;
		case "L":
		case "LZSS":
			doLZSS(in, out);
			break;
		}
		rdr.close();
		in.close();
		out.close();
	}
	
}
