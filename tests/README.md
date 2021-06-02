These tests work with bit vectors of size 4.  

**simp_lin** contains the following examples:  
* 0.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:15 < x">  
* 1.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y">  
* 2.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:3\cdot y\leq x\wedge x\leq 7\cdot y">  
* 3.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:y\leq x\wedge 2\leq x\wedge z\leq x">   
* 4.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 2\wedge 3\leq x">  
* 5.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:3\cdot y\leq x\wedge x\leq 12\cdot y">  
* 6.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 3\cdot y\wedge z\leq x\wedge x\leq t">  
* 7.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y %2B z">  
* 8.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 5\cdot y %2B 8">  
* 9.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 2\cdot y %2B z\wedge 10\cdot y\leq x">  
* 10.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 5\cdot y %2B 7\wedge 8\cdot (y %2B z)\leq x">  
* 11.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y %2B ((9 %2B 2) %2B z)">  
* 12.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:y %2B 15 < x\wedge x < 1">  
* exp1.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq (1\ll y)">  
* exp2.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:3\cdot (1\ll y)\leq x\wedge x\leq 7\cdot (1\ll y)">  
* exp3.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:(1\ll y)\leq x\wedge 2\leq x\wedge z\leq x">   
* exp5.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:3\cdot (1\ll y)\leq x\wedge x\leq 12\cdot y">  
* exp6.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 3\cdot (1\ll y)\wedge (1\ll z)\leq x\wedge x\leq t">  
* exp7.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq (1\ll y) %2B z">  
* exp8.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq (1\ll y) %2B 5\cdot y %2B 8">  
* exp9.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 2\cdot (1\ll y) %2B (1\ll z)\wedge 10\cdot (1\ll y)\leq x">  
* exp10.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 5\cdot (1\ll y) %2B 7\wedge 8\cdot ((1\ll y) %2B z)\leq x">  

**simp_exp** contains the following examples:  
* 1.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:(1\ll x)\leq y">  
* 2.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:(1\ll x)\leq 11\cdot y %2B 4">  
* 3.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:(1\ll x)\leq 5\cdot (1\ll y) %2B 7">  
* 4.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:(1\ll x)\leq (1\ll y) %2B 11\cdot y %2B 4">  
* 5.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:(1\ll x)\leq y %2B 3\cdot z %2B 8">  
* 6.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:(1\ll x)\leq 7\cdot y\wedge (1\ll x)\leq z\wedge (1\ll x)\leq (1\ll t)">  

**simp_mix** contains the following examples:  
* 1.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:(1\ll x)\leq z \wedge x\leq y">  
* 2.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:(1\ll x)\leq y \wedge (3\cdot (1\ll y)\leq x\wedge x\leq 12\cdot y)">  
* 3.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:((1\ll x)\leq y %2B 3\cdot z %2B 8) \wedge ((1\ll x)\leq (1\ll y) %2B 11\cdot y %2B 4) \wedge (3\cdot (1\ll y)\leq x\wedge x\leq 12\cdot y)">  

**lin** contains the following examples:  
* 1.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:7\cdot x\leq y">  

**exp** contains the following examples:  
* 1.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:(1\ll x) %2B 2\leq y %2B 14">  
* 2.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:11\cdot (1\ll x)\leq y %2B 12">  
* 3.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:(1\ll x) %2B x\leq (1\ll y) %2B y %2B 1">  

