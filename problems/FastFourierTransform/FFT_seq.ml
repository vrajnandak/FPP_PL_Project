let pi = 4.0 *. atan 1.0

type complex = float * float

let complex_add (a, b) (c, d) = (a +. c, b +. d)
let complex_sub (a, b) (c, d) = (a -. c, b -. d)
let complex_mul (a, b) (c, d) = (a *. c -. b *. d, a *. d +. b *. c)

let omega k n =
  let angle = -.2.0 *. pi *. float_of_int k /. float_of_int n in
  (cos angle, sin angle)

let rec fft (a) (out) (start) (n) (step) =
    if n = 1 then
      Array.set out (start / step) a.(start)
    else
      let half = n / 2 in
      let even = Array.make half (0.0, 0.0) in
      let odd  = Array.make half (0.0, 0.0) in

      fft a even start half (2 * step);
      fft a odd (start + step) half (2 * step);

      for k = 0 to half - 1 do
        let w = complex_mul (omega k n) odd.(k) in
        out.(k) <- complex_add even.(k) w;
        out.(k + half) <- complex_sub even.(k) w
      done

let () =
  let n = 1 lsl 20 in
  let input = Array.make n (1.0, 0.0) in
  let output = Array.make n (0.0, 0.0) in

  let t0 = Unix.gettimeofday () in

  fft input output 0 n 1;

  let t1 = Unix.gettimeofday () in

  Printf.printf "Sequential FFT done in: %.6f seconds\n%!" (t1 -. t0)
