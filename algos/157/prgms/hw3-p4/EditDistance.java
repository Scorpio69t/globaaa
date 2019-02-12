import java.io.*;

public class EditDistance {

	/* print out a table */
	public static void debug(int[][] array, int n, int m) {
		for(int i = 0; i < n; i++) {
			for(int k = 0; k < m; k++) {
				System.out.print(array[i][k]);
				System.out.print(" ");
			}
			System.out.print("\n");
		}
	}

	/* different orderings for 3-way min */
	public static int min1(int c1, int c2, int c3) {
		return Math.min(Math.min(c1, c2), c3);
	}

	public static int min2(int c1, int c2, int c3) {
		return Math.min(Math.min(c1, c3), c2);
	}

	public static int min3(int c1, int c2, int c3) {
		return Math.min(Math.min(c2, c3), c1);
	}

	/* Basic implementation straight from pseudocode; practice with Java
	 * and used sometimes as a baseline against the correctness of optimized
	 * versions. */
	public static int dist0(String s1, String s2)  {
		int s1_len = s1.length();
		int s2_len = s2.length();
		int[][] table = new int[s1_len + 1][s2_len + 1];

		/* initialize base cases  */
		for(int i = 0; i <= s1_len; i++) {
			table[i][0] = i;
		}

		for(int j = 0; j <= s2_len; j++) {
			table[0][j] = j;
		}

		/* iterate left to right, top to bottom. if you change the order of (i, j)
		 * it'll substantially slow everything down; shows the cost of too many
		 * cache misses */
		for(int i = 1; i <= s1_len; i++) {
			for(int j = 1; j <= s2_len; j++) {
				if (s1.charAt(i-1) == s2.charAt(j-1)) {
					table[i][j] = table[i-1][j-1];
				} else {
					table[i][j] = 1 + Math.min(Math.min(table[i-1][j], table[i][j-1]), table[i-1][j-1]);
				}
			}
		}

		return table[s1_len][s2_len];
	}

	/*
	 * Implemented the space-saving algorithm as described in part 1
	 */
	public static int dist1(String s1, String s2)  {
		int len = s1.length();

		/* instead of creating a len by len table, let's instead
		 * store only 2 rows */
		int[] r1 = new int[len + 1];
		int[] r2 = new int[len + 1];
		int[] tmp;

		/* initialize base cases  */
		for(int j = 0; j <= len; j++) {
			r1[j] = j;
		}

		r2[0] = 1;

		for(int i = 1; i <= len; i++) {
			for(int j = 1; j <= len; j++) {
				if (s1.charAt(i-1) == s2.charAt(j-1)) {
					r2[j] = r1[j-1];
				} else {
					r2[j] = 1 + min1(r1[j], r1[j-1], r2[j-1]);
				}
			}

			/* since arrays are passed by reference, this'll simply swap the
			 * the pointers between the arrays; we'll only ever use 2*len
			 * memory. */
			tmp = r1;
			r1 = r2;
			r2 = tmp;
			r2[0] = i + 1;
		}

		return r1[len];
	}

	/*
	 * Implemented the further optimized algorithm as described in part 3
	 */
	public static int dist3(String s1, String s2)  {
		int len = s1.length();

		int[] r = new int[len + len + 1];

		for(int j = 0; j < len; j++) {
			r[j] = len - j;
		}

		for(int i = len; i <= (len + len); i++) {
			r[i] = i - len;
		}

		for(int i = len; i > 0; i--) {
			for(int j = 0; j < len; j++) {
				if (s1.charAt(len - i) == s2.charAt(j)) {
					continue;
				} else {
					r[i + j] = 1 + min1(r[i + j - 1], r[i + j], r[i + j + 1]);
				}
			}
		}

		return r[len];
	}

	/*
	 * Implemented the diagonal algorithm as described in part 6; assume strings are equal lengths
	 */
	public static int dist6(String s1, String s2)  {
		int len = s1.length();
		int tmp[];

		/* we'll use three arrays instead of a table */
		int[] r1 = new int[len + 1];
		int[] r2 = new int[len + 1];
		int[] r3 = new int[len + 1];

		/* base cases */
		r1[0] = 0;
		r2[0] = 1;
		r2[1] = 1;

		for(int i = 1; i < len; i++) {
                        /* assign ends of diagonal rows */
                        r3[0] = i + 1;
                        r3[i + 1] = i + 1;

			for(int j = i, k = 1; j > 0; j--, k++) {
				if (s1.charAt(k-1) == s2.charAt(j-1)) {
					r3[j] = r1[j-1];
				} else {
					r3[j] = 1 + min1(r1[j-1], r2[j-1], r2[j]);
				}
			}

			/* update rows */
			tmp = r1;
			r1 = r2;
			r2 = r3;
			r3 = tmp;
		}

                for(int i = 1; i <= len; i++) {
                        for(int j = len, k = i; j >= i; j--, k++) {
                                if (s1.charAt(k-1) == s2.charAt(j-1)) {
                                        r3[j] = r1[j-1];
                                } else {
                                        r3[j] = 1 + min1(r1[j-1], r2[j-1], r2[j]);
                                }
                        }

			tmp = r1;
			r1 = r2;
			r2 = r3;
			r3 = tmp;
		}

		return r2[len];
	}

	public static void main(String[] args) {
		boolean testingMode = (args.length > 0 && args[0].equals("-test"));
		BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
		String s1, s2;
		try {
			if (testingMode) {//run your code with the "-test" flag to enter inputs manually
				s1 = reader.readLine().replace("\n","").replace("\r","");
				s2 = reader.readLine().replace("\n","").replace("\r","");

				System.out.println(dist1(s1, s2));
				System.out.println(dist3(s1, s2));
				System.out.println(dist6(s1, s2));
			} else {//otherwise, generate and test on length 10000 inputs
				long start, end;
				int n=10000, m=10000;

				double p = .5;

				String a = "a";
				String b = "b";

				StringBuilder sb1 = new StringBuilder(n);
				for (int i = 0; i < n; i++){
					if (Math.random() < p){
						sb1.append(a);
					}else{
						sb1.append(b);
					}
				}

				StringBuilder sb2 = new StringBuilder(m);
				for (int i = 0; i < m; i++){
					if (Math.random() < p){
						sb2.append(a);
					}else{
						sb2.append(b);
					}
				}

				s1=sb1.toString(); s2=sb2.toString();
				System.out.println("random strings:");

				start = System.currentTimeMillis();
				System.out.println("dist1 answer: " + EditDistance.dist1(s1,s2));
				end = System.currentTimeMillis();
				System.out.println("time: " + (end-start) + " ms");

				start = System.currentTimeMillis();
				System.out.println("dist3 answer: " + EditDistance.dist3(s1,s2));
				end = System.currentTimeMillis();
				System.out.println("time: " + (end-start) + " ms");

				start = System.currentTimeMillis();
				System.out.println("dist6 answer: " + EditDistance.dist6(s1,s2));
				end = System.currentTimeMillis();
				System.out.println("time: " + (end-start) + " ms");
				System.out.println();


				sb1 = new StringBuilder(n);
				for (int i = 0; i < n; i++){
					sb1.append(a);
				}

				sb2 = new StringBuilder(m);
				for (int i = 0; i < m; i++){
					sb2.append(b);
				}

				s1=sb1.toString(); s2=sb2.toString();
				System.out.println("opposite strings:");

				start = System.currentTimeMillis();
				System.out.println("dist1 answer: " + EditDistance.dist1(s1,s2));
				end = System.currentTimeMillis();
				System.out.println("time: " + (end-start) + " ms");

				start = System.currentTimeMillis();
				System.out.println("dist3 answer: " + EditDistance.dist3(s1,s2));
				end = System.currentTimeMillis();
				System.out.println("time: " + (end-start) + " ms");

				start = System.currentTimeMillis();
				System.out.println("dist6 answer: " + EditDistance.dist6(s1,s2));
				end = System.currentTimeMillis();
				System.out.println("time: " + (end-start) + " ms");
				System.out.println();

				sb1 = new StringBuilder(n);
				for (int i = 0; i < n; i++){
					sb1.append(a);
				}

				sb2 = new StringBuilder(m);
				for (int i = 0; i < m; i++){
					sb2.append(a);
				}

				s1=sb1.toString(); s2=sb2.toString();
				System.out.println("identical strings:");

				start = System.currentTimeMillis();
				System.out.println("dist1 answer: " + EditDistance.dist1(s1,s2));
				end = System.currentTimeMillis();
				System.out.println("time: " + (end-start) + " ms");

				start = System.currentTimeMillis();
				System.out.println("dist3 answer: " + EditDistance.dist3(s1,s2));
				end = System.currentTimeMillis();
				System.out.println("time: " + (end-start) + " ms");

				start = System.currentTimeMillis();
				System.out.println("dist6 answer: " + EditDistance.dist6(s1,s2));
				end = System.currentTimeMillis();
				System.out.println("time: " + (end-start) + " ms");
				System.out.println();
			}
		} catch (IOException e) {
			System.err.println("Error reading input strings.");
		}
	}
}
