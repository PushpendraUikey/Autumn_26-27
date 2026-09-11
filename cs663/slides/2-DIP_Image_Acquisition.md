# 📸 CS-663: Digital Image Processing
## Week 2 Masterclass — Digital Image Acquisition

> *"Before you can process an image, you must understand how light becomes a number."*

Welcome, student. Today we trace the full journey of a photograph: from photons bouncing off the real world, through a tiny hole or a lens, onto a grid of silicon sensors, and finally into the rows and columns of numbers you'll manipulate all semester. Grab a coffee ☕ — this is a rich one.

---

## 🗺️ The Learning Journey

1. Digital Images as Functions
2. Image Formation — The Pinhole Camera
3. Vanishing Points & Perspective Geometry
4. Image Sensing — CCD/CMOS & Bayer Filters
5. Sampling vs. Quantization
6. Color Spaces & Engineering
7. 🧪 Practice Problem Set (Easy → Hard)
8. ✅ Full Worked Solutions

---

## 1️⃣ Digital Image Basics: Images as Functions

> 💡 **Big Idea:** A digital image is nothing but a **function**. Forget "picture" for a second — think of it as a machine that takes in a *location* and spits out a *value*.

* **Domain** = *space* — the coordinates you're allowed to plug in.
  * A 2D photograph → domain is a 2D grid `(row, col)`
  * A 3D medical scan (CT/MRI volume) → domain is a 3D grid `(row, col, slice)`
* **Range** = *what comes out* — scalar or vector.
  * Black-and-white photo → **scalar** (one brightness number per pixel)
  * Color photo → **vector** `[R, G, B]` (three numbers per pixel)
  * Video → a function of space *and* time

**🎨 Analogy:** Think of a digital image like a *spreadsheet of temperature readings across a city*. Each cell (domain = latitude/longitude grid) holds a temperature (range = scalar value). A color photo is the same spreadsheet, except each cell holds *three* readings instead of one — like recording temperature, humidity, *and* wind speed at every point.

**Colormaps** are just a "paint-by-numbers" lookup table: the same underlying scalar data array can be *visualized* very differently depending on which colormap (grayscale, jet, viridis...) you apply. The data doesn't change — only the "translation" into color does.

### 🌈 RGB vs. Multispectral vs. Hyperspectral

| Imaging Type | # of Bands | Analogy |
|---|---|---|
| **RGB** | 3 (Red, Green, Blue) | Tasting a dish and noting *sweet, sour, salty* only |
| **Multispectral** | ~4–15 (often includes infrared, etc.) | Noting *sweet, sour, salty, bitter, umami* |
| **Hyperspectral** | Hundreds, across a continuous slice of the electromagnetic spectrum | Running the dish through a full chemical assay — measuring *every* molecule's contribution |

A hyperspectral "image" is really a **cube**: two spatial dimensions plus one spectral dimension (wavelength). Slicing it at one wavelength gives an ordinary grayscale image; stacking all the slices gives the full spectral fingerprint of every pixel — useful for satellite imagery, mineral detection, and food inspection, where color alone (RGB) hides information that a wider slice of the spectrum reveals.

---

## 2️⃣ Image Formation: The Pinhole Camera

### 🕳️ Camera Obscura — Nature's First Camera

In Latin, *camera obscura* literally means **"dark chamber."** It's a natural phenomenon: when light rays pass through a tiny hole into a dark space, they form an image on the surface they strike — no lens required!

> **Why does the image flip?** Picture a ray of light leaving the *top* of a tree outside. It travels in a straight line, passes through the pinhole, and — because it can't bend — it keeps going straight, landing near the *bottom* of the back wall. Meanwhile a ray from the bottom of the tree lands near the *top*. The result: the projected image is **inverted (up↔down) and reversed (left↔right)**.

This isn't just theory — it happens in real buildings by accident! A hole in a tiled roof at the New Royal Palace of Prague Castle has projected the outside world onto an attic wall. Artists in the 1700s–1800s even carried **portable camera-obscura tents** into the field, tracing the projected (inverted) scene onto paper to sketch with perfect perspective.

### 📐 The Geometry: From 3D World to 2D Image

Now let's make it mathematical. Set up the pinhole camera:

* The **pinhole** sits at the origin `O = (0, 0, 0)`
* The **image plane** sits a distance `f` (focal length, f > 0) *behind* the pinhole
* A point in the 3D world has signed coordinates `(x1, x2, x3)`
* Its projection onto the image plane has 2D coordinates `(y1, y2)`

**🎨 Analogy:** Think of the pinhole as a *strict bouncer* at a club door. Every ray of light from the outside world must pass through that one exact point to get "in." Once it's through, it just keeps travelling in a straight line until it hits the back wall (image plane). Since *all* rays are funneled through one single point, the geometry becomes pure similar-triangles trigonometry.

Using similar triangles (top-view and side-view analysis), for a scene point `(x1, x2, x3)` with `x3 > 0`, the ray through the pinhole hits the *real* (inverted) image plane at:

```
y1 = -f · x1 / x3
y2 = -f · x2 / x3
```

The minus sign is exactly the mathematical signature of that flip/inversion we saw in the camera obscura! Notice — every point *anywhere* along the ray from O through the scene point P projects to this *same* `(y1, y2)`. This is the essence of **perspective projection**: an entire 3D ray collapses onto a single 2D point.

### 🪞 The Virtual Image Plane — Undoing the Flip

Working with negative, inverted coordinates all the time is annoying for analysis. So we introduce a mathematical trick: the **virtual image plane**, placed a distance `f` in *front* of the pinhole instead of behind it.

* `(-y1)` becomes `(+y1)`
* `(-y2)` becomes `(+y2)`

This avoids the inversion/reversal entirely during analysis (nobody's *building* a camera this way — it's purely a bookkeeping convenience), giving the friendlier, more commonly quoted form:

```
y1 = f · x1 / x3
y2 = f · x2 / x3
```

**🎨 Analogy:** It's like designing a video game camera. You *could* simulate light literally punching through a hole and flipping onto a wall behind it — or you could just place your "screen" symbolically in front of the camera and skip the headache of un-flipping everything later. Same math, friendlier bookkeeping.

### 🔗 A Small but Powerful Detour: Homogeneous Coordinates

Ordinary `(x, y)` coordinates can't represent "points at infinity" — but projective geometry (and perspective projection) cares deeply about such points (parallel lines "meeting" far away, sunlight arriving as parallel rays, etc.).

**The trick:** represent a 2D point `(x, y)` as a whole *family* `{(x·S : y·S : S) : for any nonzero scale S}`. To recover the point, divide: given homogeneous coordinates `(p : q : r)`, the actual point is `(p/r, q/r)`.

The magic: consider a line through the origin, `bt, -at`, parametrized by `t`. Its homogeneous coordinates are `(bt : -at : 1)`, which is equivalent to `(b : -a : 1/t)`. As `t → ∞` (moving infinitely far along the line), this becomes `(b : -a : 0)` — a perfectly well-defined homogeneous coordinate, even though no ordinary `(x, y)` pair could represent "infinity." This lets us model vanishing points (see next section) as ordinary algebra instead of a special case.

**🎨 Analogy:** It's like adding a "zoom level" dial to your coordinate system. Normally coordinates only describe *where* you are. Homogeneous coordinates add a *how zoomed in* dimension (`S`), and when you zoom all the way out to `S = 0`, you can still meaningfully talk about *which direction* you were heading — even though you never quite "arrive."

### 🚂 Vanishing Points: Where Parallel Lines "Meet"

Everyone has seen this: train tracks running toward the horizon appear to converge to a single point. Let's prove *why*, with real math from the geometry above.

Take a 3D line parametrized by `t`:

```
x1(t) = d1 + a·t
x2(t) = d2 + b·t
x3(t) = d3 + c·t
```

Its perspective projection is:

```
y1(t) = (d1 + a·t) · f / (d3 + c·t)
y2(t) = (d2 + b·t) · f / (d3 + c·t)
```

Now let `t → ∞`. Dividing numerator and denominator by `t` and taking the limit, the offsets `(d1, d2, d3)` — i.e., *where* the line starts — cancel out completely:

```
y1(t) → a·f / c
y2(t) → b·f / c        (assuming c ≠ 0)
```

**This is the punchline:** *any* line with direction `(a, b, c)` — no matter where it starts in 3D space — projects to the *exact same* vanishing point `(af/c, bf/c)`. That's why two parallel railway tracks starting miles apart still appear to converge to one single point on the horizon: only the *direction* of the line matters for the vanishing point, not its position.

**Bonus proof — the projected path really is a straight line.** Differentiating `y1(t)` and `y2(t)` with respect to `t` and computing the slope `dy2/dy1` shows the slope is **independent of `t`** — meaning the projected curve has constant slope everywhere, i.e., it's a straight line. (Perspective projection always maps 3D straight lines to 2D straight lines — one of its most important properties.)

**🎨 Analogy:** Imagine two friends walking in *exactly* the same compass direction, starting from different houses. From a satellite view directly above, their paths look parallel and never meet. But if you instead watch from a low camera *on the ground* far ahead of them, both paths appear to converge toward the same point on your horizon — because what matters to your 2D view is only their shared *direction*, not their different starting *positions*.

**Special case — what if `c = 0`?** That means the line is *parallel to the image plane itself* (never gets nearer or farther, so `x3` stays constant `= d3`). In this case there is no vanishing point at all — the projected slope becomes `b/a`, *exactly* the same as the 3D line's own slope, and parallel 3D lines project to parallel 2D lines that never meet (think of the horizontal lines of a distant fence running left-right in front of you, staying parallel in the photo).

### 🔺 Other Perspective Projection Properties

1. **Straight lines → straight lines.** Any planar triangle `A-B-P` in 3D, sliced by the image plane, produces a straight line segment in the 2D image.
2. **Spheres → ellipses** (in general) — a ball photographed off-center in an image doesn't project to a perfect circle, but to an ellipse.

### 🔦 Pinhole Limitations & Why We Use Lenses

* **Small pinhole** → sharp image, but very **dark** (little light gets through)
* **Large pinhole** → **brighter** image, but **blurry** (many overlapping rays smear the projection)

This is a fundamental trade-off pinhole cameras can't escape. **Real cameras replace the pinhole with a lens**: lighter, sharp *and* bright, because a lens can gather light from a wide area and still focus it to a point. A thin, symmetric lens is modeled as if all rays cross through a single virtual plane at its center — this *approximates* pinhole geometry, but (unlike an idealized pinhole) doesn't perfectly account for geometric distortion or the fact that different colors refract slightly differently through real glass.

### 👁️ Field of View (FOV)

**Field of view** (or angle of view) is the angular extent of the world a camera can "see." For reference, human vision spans roughly **120° horizontally** (binocular vision) by **60° vertically** (central vision). As you zoom a camera lens in, FOV *reduces* — you see a narrower slice of the world, magnified.

---

## 3️⃣ Image Sensing & Bayer Filters

### ⚡ From Photons to Numbers

Modern cameras use one of two sensor technologies, both built on **MOSFET** (metal-oxide-semiconductor field-effect transistor) technology:

* **CCD** (Charge-Coupled Device)
* **CMOS / Active-Pixel Sensor**

Both do the same fundamental job: **convert photon hits → electric current → voltage.** The image sensor is a 2D array of these light-sensitive elements.

**How CCD works, specifically:** the image is projected through a lens onto an array of *capacitors*. Each capacitor accumulates electric charge in direct proportion to the light intensity falling on it — brighter light, more accumulated charge, higher eventual voltage reading.

**🎨 Analogy:** Picture a grid of tiny rain buckets left out in a storm. Each bucket (capacitor/pixel) collects rainwater (photons) for a fixed period. A bucket under a heavy downpour (bright light) fills up much more than one under a light drizzle (dim light). At the end, you measure how much water is in each bucket — that's your brightness value.

### 🟥🟩🟦 Color Separation: The Bayer Filter

Here's the catch: a bare capacitor can't tell *what color* photon it just absorbed — only *how many*. So how do cameras get color?

**Solution:** overlay a mosaic of tiny color filters — one per sensor element — called a **Bayer pattern** (invented by Bryce Bayer at Eastman Kodak, 1975).

The most common arrangement is **RGGB**:

```
R  G
G  B
```

repeated across the whole sensor grid.

**❓ Why twice as many green elements as red or blue?**

The Bayer pattern deliberately mimics the physiology of the **human eye**, which is far more sensitive to green wavelengths (they sit closest to the peak of our luminance perception). By dedicating 50% of the sensor's elements to green, the sensor captures the most *perceptually important* detail — the information our eyes weight most heavily for perceived sharpness and brightness — at the highest possible resolution.

**🎨 Analogy:** Imagine you're building a team of 4 quality inspectors to check a factory line, and you know from experience that "brightness defects" are what customers notice 60% of the time, "redness defects" 20%, and "blueness defects" 20%. You'd staff 2 inspectors on brightness and 1 each on the other two — exactly what RGGB does for green vs. red/blue.

After capture, each sensor location only knows *one* of R, G, or B — never all three. Two paths forward:

1. **Sub-sample:** just accept a lower-resolution RGB image (one full R/G/B pixel triplet built from a small neighborhood).
2. **Demosaic (de-Bayer) / interpolate:** estimate the two missing color channels at every pixel location from neighboring sensor readings, producing a full-resolution RGB image.

The end-to-end pipeline: *(1) original scene → (2) raw sensor output through the Bayer filter → (3) that raw output visualized with its filter colors → (4) reconstructed image after interpolation → (5) full "true" RGB for comparison.*

---

## 4️⃣ Sampling vs. Quantization — Two *Different* Kinds of "Rounding"

These two concepts get mixed up constantly. Keep them **strictly separate**:

| | **Spatial Sampling** | **Intensity Quantization** |
|---|---|---|
| **What's being limited?** | *Where* you take measurements (the domain) | *What values* a measurement can take (the range) |
| **Cause** | Finite number of sensor elements / pixels | Finite number of bits per pixel |
| **Failure mode** | **Moiré patterns**, aliasing, loss of fine spatial detail | Visible banding, posterization, rounding error |

### 📏 Spatial Sampling

An image is stored at a **finite** set of discrete locations because we only have finite memory — you can't record brightness at *every* infinitesimal point in space, so you place a grid of sensors and record one value per grid cell.

There's a direct engineering trade-off here: for the same physical sensor chip and exposure time, **packing in more sensor elements reduces the field-of-view each individual sensor covers** ("per-pixel FOV"). More pixels = finer sampling, but each one gathers less light (weaker signal) and tiny sensors are also physically harder to manufacture reliably.

**🎨 Analogy:** Sampling is like taking a survey of a city's opinion by knocking on doors. If you only knock on every 10th house (**undersampling**), you might completely miss a pattern that repeats every 7 houses — and worse, you might *misread* it as some other, slower-changing pattern entirely. That misreading is exactly what causes...

**⚠️ Moiré Patterns:** When you undersample a scene that has fine, repetitive detail (a striped shirt, a chain-link fence, roof tiles), the coarse sampling grid can't tell the difference between the *true* fine pattern and certain *other*, much coarser patterns — so a false, swirling interference pattern appears in the image that was never actually in the scene. It's a genuine loss/corruption of information caused purely by not sampling densely enough.

**🎨 Analogy for Moiré:** Ever seen a spinning helicopter rotor look like it's moving *slowly backward* on video, even though it's spinning fast? That's the *temporal* version of the exact same undersampling problem — the camera's frame rate (temporal sampling rate) is too slow to correctly capture the rotor's true rotation speed, so your eye is fooled into perceiving a false, slower rotation.

### 🎚️ Intensity Quantization

*After* spatial sampling has fixed *where* you measure, quantization limits *how many distinct values* each measurement can be recorded as.

* This is literal **rounding** of real (or high-precision integer) brightness values down to a limited set of allowed levels.
* **Why do we need it?** Same reason as sampling — finite memory per pixel. Common budgets: 8 bits, 16 bits, 24 bits, 32 bits.
* With **N bits**, you get **2^N** possible intensity levels (e.g., 8 bits → 256 levels, the classic 0–255 range).

**🎨 Analogy:** Quantization is like a school converting a continuous 0–100% exam score into a **letter grade** (A, B, C, D, F). Two students who scored 91% and 89% might both get rounded into different discrete "bins" (A vs. B) even though their true performance was nearly identical — that's the rounding error inherent to quantization, completely separate from *how many students you tested* (which would be the sampling question).

---

## 5️⃣ Color Spaces & Engineering

### ➕ Additive (RGB) vs. ➖ Subtractive (CMY)

| | **RGB (Additive)** | **CMY (Subtractive)** |
|---|---|---|
| **Starting point** | Black (no light) | White (paper, reflecting all light) |
| **Mechanism** | *Add* colored light together | *Subtract* (absorb) colored light with ink |
| **Used for** | Screens, TVs, digital cameras (emitting light) | Printing (reflecting ambient light) |
| **Primaries** | Red, Green, Blue | Cyan, Magenta, Yellow |

**RGB — Additive:** you start with total darkness and *build up* a color by adding red, green, and blue light in specific amounts. The RGB "color cube" has black at the corner `(0,0,0)` and white at the opposite corner `(1,1,1)`. Colors on the *outer surface* of the cube (where at least one channel is at its maximum) are called **saturated colors**.

**CMY — Subtractive:** printing works completely differently — there's no light source *inside* the page, only ambient light reflecting off it. Each ink pigment works by **absorbing (subtracting)** a portion of the reflected light spectrum:

* **Cyan** ink absorbs **red** light → reflects green + blue (`C = B + G` in RGB terms)
* **Magenta** ink absorbs **green** light → reflects red + blue (`M = R + B`)
* **Yellow** ink absorbs **blue** light → reflects red + green (`Y = R + G`)

The direct conversion formula from RGB (each channel normalized to 0–1) to CMY:

```
C = 1 - R
M = 1 - G
Y = 1 - B
```

* **White** = `C = M = Y = 0` → no ink at all, paper reflects everything.
* **Black** = `C = M = Y = 1` → complete saturation of all three inks (in practice this isn't perfectly black on real printers, which is exactly why a dedicated black-ink cartridge (K, giving CMYK) is added).

**🎨 Analogy:** RGB is like mixing colored spotlights on a dark stage — shine red + green + blue spotlights all at once on the same spot and you get white light. CMY is like painting with colored *filters* over a white sheet of paper in daylight — every filter you add can only block *more* light, never add any, so stacking all three filters gets you toward black.

### 🔲 Grayscale / Luminance: Why the Weights Are Unequal

The simplest way to turn RGB into a single gray value is a plain average: `A = (R + G + B) / 3`. But this is perceptually *wrong* — humans don't perceive red, green, and blue as equally "bright." Our eyes are most sensitive to **green**, next to **red**, and least to **blue**.

So instead we take a **convex combination with unequal weights**, giving the largest weight to green and the smallest to blue:

* **Analog television (Rec. 601):**
  ```
  Y = Lum(R,G,B) = 0.2990·R + 0.5870·G + 0.1140·B
  ```
* **Digital devices (Rec. 709):**
  ```
  Y = Lum(R,G,B) = 0.2125·R + 0.7154·G + 0.0721·B
  ```

**🎨 Analogy:** This is exactly like a teacher weighting a final grade: instead of a flat average of homework, midterm, and final, they might weight it 20% / 30% / 50% because the final exam is a *better indicator* of true understanding. Here, "green" is the "final exam" of color perception — it tells your eye the most about perceived brightness, so it earns the biggest weight.

### 🖌️ Desaturation — A Practical Use of Luminance

To reduce a color's saturation, you blend it *toward* its own gray/luminance value:

```
[Rd, Gd, Bd] = [A, A, A] + s · [R-A, G-A, B-A]
             = s·[R,G,B] + (1-s)·[A,A,A]        where A = (R+G+B)/3
```

This is a **convex combination** of the original color `[R,G,B]` and its gray equivalent `[A,A,A]`, controlled by a slider `s`:

* `s` between 0 and 1 → **desaturation** (moves *toward* gray)
* `s > 1` → **increased** saturation (moves *away* from gray, with values clamped to a valid range)

### 🍦 HSV & HSL — Thinking Like a Painter

RGB is great for hardware, but terrible for *human intuition* — nobody thinks "let's make this shirt a bit more (0.2, 0.6, 0.1)." Painters and designers think in terms of **Hue** (which color), **Saturation** (how vivid), and **Value/Lightness** (how bright/dark). HSV and HSL are exactly this, formalized in the 1970s for computer graphics and still used today in color pickers and image editors.

* **HSV** (Hue, Saturation, Value) — maps naturally to a **cylinder**, or, restricted to valid colors, a **cone**:
  * The **central vertical axis** is the neutral/achromatic gray-scale line — pure white at the top, pure black at the bottom.
  * Pure, fully-saturated colors sit around the *top* outer rim; moving down the outer surface, colors get progressively darker.
  * Saturation (S) corresponds to how far out from the central axis you are — sometimes called **chroma**.
* **HSL** (Hue, Saturation, Lightness) — maps to a **double cone** (two cones joined base-to-base):
  * White is at the very top point, black at the very bottom point, with maximally saturated colors bulging out around the *middle*.

In both spaces:

1. The **central axis** = neutral grays (achromatic).
2. The additive primaries (RGB) and additive secondaries (CMY) sit on the *outer surface*, at Value = 1 (HSV) or Lightness = 0.5 (HSL).

**🎨 Painter's Terminology — Tint, Tone, Shade:**

* **Tint** = pure color + **white** (moving *up* toward the light end — think pastels)
* **Shade** = pure color + **black** (moving *down* toward the dark end — think deep, moody colors)
* **Tone** = pure color + **gray** (mixing toward the central neutral axis — think muted, "dusty" colors)

Picture a strip of blue: the top half — progressively mixed with white — gives you *tints* of blue (baby blue, powder blue); the bottom half — progressively mixed with black — gives you *shades* of blue (navy, midnight blue). This is precisely the geometry of moving up/down the HSV/HSL cone.

### 📺 YUV / YCbCr — Engineering Color for Compression

These color spaces exist for one core engineering reason: **separate the luminance (Y) from the two chroma (color) components**, because:

* (a) The **human visual system is far more sensitive to detail in lightness than in color** — we notice a blurry color patch far less than a blurry brightness edge.
* (b) It preserves **compatibility with legacy black-and-white systems** — a black-and-white TV can just read the Y channel and ignore the rest.
* (c) It lets engineers **allocate bandwidth/storage asymmetrically** — spend more bits on Y (which matters more to perception), fewer bits on the chroma channels (which matter less).

**YUV** (analog TV — NTSC in North America, PAL in Europe):

```
Y  = luminance (same weighted formula as before)
U  = scaled version of (B - Y)   →  "blueness minus brightness"
V  = scaled version of (R - Y)   →  "redness minus brightness"
```

**YCbCr** (digital TV, and JPEG compression) is closely related, using the same idea with the Rec. 601 luminance weights:

```
wR = 0.299,  wG = 0.587,  wB = 0.114
Cb ≈ scaled (B - Y)
Cr ≈ scaled (R - Y)
```

**One subtlety worth knowing:** *Y* (luminance) is technically defined from *linear* RGB. In practice, most real systems apply a nonlinear "gamma correction" to RGB first (denoted `R'G'B'`), and the resulting luminance-like quantity computed from *that* is properly called **luma (Y′)** rather than luminance (Y). So `Y′` is a nonlinear function of `Y`, `R′` a nonlinear function of `R`, and so on — a distinction papers/textbooks are often loose about, but worth knowing precisely.

### 🎞️ Chroma Subsampling — 4:2:2 and 4:2:0

Since chroma detail matters less to our eyes than luminance detail, engineers **literally throw away** some chroma resolution to save bandwidth — this is **chroma subsampling**.

**Notation `4:X:Y`** describes how many chroma (CbCr) samples are recorded per 4 luminance samples, across two rows:

* **4:4:4** — full chroma resolution, no subsampling (every pixel gets its own Cb, Cr)
* **4:2:2** — record CbCr for only **2** out of every 4 pixels in **row 1**, and **2** out of every 4 pixels in **row 2** → half the horizontal chroma resolution
* **4:2:0** — record CbCr for **2** pixels in row 1, but **0** (none, reuse row 1's values) in row 2 → half horizontal *and* half vertical chroma resolution

**🎨 Analogy:** Think of describing a rainbow-colored crowd photo to a friend over a slow phone connection. You'd read out every single person's *exact brightness/silhouette* (that's Y — full resolution, because sharp edges matter most for recognizing the scene) but you might only mention the *general color zone* every couple of people ("this cluster is reddish, that cluster is bluish") rather than each individual's precise hue — because losing a *little* color precision is barely noticeable, while losing brightness detail would make the photo unrecognizable. That's exactly the trade TV broadcast and JPEG compression make with 4:2:2 / 4:2:0.

This single design decision (spend bits on Y, subsample CbCr) is one of the most important, oldest tricks in image and video compression — it directly enables both **TV bandwidth reduction** and **JPEG's compression ratio**.

---

## 6️⃣ 🧪 Practice Problem Set

> Try to solve each problem *before* scrolling to the solutions at the end. No LaTeX — write your work in plain text just like the problems below.

### 🟢 Problem 1 (Conceptual — Easy)
**Pinhole Aperture Size**

Suppose you have a basic pinhole camera. If you **increase the size of the pinhole aperture**, what happens to:

**(a)** the **brightness** of the resulting image, and
**(b)** the **sharpness** of the resulting image?

Explain *why*, in terms of what's physically happening to the light rays passing through the hole.

---

### 🟡 Problem 2 (Applied Math — Medium)
**Perspective Projection**

A pinhole camera has its pinhole at the origin, with a **virtual image plane** at focal length **f = 5 cm** in front of it.

A point in the 3D scene is located at:

```
(x1, x2, x3) = (12 cm, 6 cm, 24 cm)
```

Using the standard perspective projection equations for the **virtual image plane**:

```
y1 = f · x1 / x3
y2 = f · x2 / x3
```

**Calculate the exact 2D projection coordinates (y1, y2)** of this point on the image plane.

---

### 🔴 Problem 3 (Color Engineering — Hard)
**Luminance and CMY Conversion**

You are given a normalized RGB pixel value (each channel scaled to the 0–1 range):

```
R = 0.80,   G = 0.40,   B = 0.20
```

**(a)** Calculate the **Luminance (Y)** of this pixel using the standard **analog television (Rec. 601)** weights:
```
Y = 0.2990·R + 0.5870·G + 0.1140·B
```

**(b)** Using the standard subtractive color model, calculate the corresponding **pure CMY** print values for this pixel:
```
C = 1 - R
M = 1 - G
Y_ink = 1 - B
```
(Note: use `Y_ink` for the yellow ink channel here, to avoid confusing it with luminance `Y` from part (a).)

---
---

<br><br>

> ## 🔒 SOLUTIONS BELOW — No peeking until you've attempted all three! 🔒

<br><br>

---
---

## ✅ Solutions

### Solution 1 — Pinhole Aperture Size (Easy)

**(a) Brightness:** Increasing the pinhole size **increases brightness**. A bigger hole lets *more total light rays* pass through per unit time onto any given point of the sensor/film, so more photons accumulate → higher signal → brighter image.

**(b) Sharpness:** Increasing the pinhole size **decreases sharpness** (image becomes blurrier). With a tiny/ideal pinhole, essentially only *one* ray from each point in the scene can reach any given point on the image plane — a clean, sharp one-to-one mapping. As the hole gets bigger, *many* slightly different rays from the *same* scene point can pass through different parts of the (now large) opening and land on *slightly different* points on the image plane. These overlapping, smeared contributions blur what should have been one crisp point into a small blob — reducing sharpness.

**Takeaway:** this is a genuine trade-off inherent to pinhole geometry — you cannot get both maximum brightness *and* maximum sharpness from a pinhole alone. (This is precisely why real cameras replace the pinhole with a *lens*: a lens can gather light from a wide opening while still refracting/focusing all those rays back down to a single sharp point.)

---

### Solution 2 — Perspective Projection (Medium)

**Given:**
```
f = 5 cm
(x1, x2, x3) = (12 cm, 6 cm, 24 cm)
```

**Step 1 — Apply the y1 formula:**
```
y1 = f · x1 / x3
y1 = 5 · 12 / 24
y1 = 60 / 24
y1 = 2.5 cm
```

**Step 2 — Apply the y2 formula:**
```
y2 = f · x2 / x3
y2 = 5 · 6 / 24
y2 = 30 / 24
y2 = 1.25 cm
```

**Final Answer:**
```
(y1, y2) = (2.5 cm, 1.25 cm)
```

**Sanity check:** notice the scene point is twice as far along `x1` as `x2` (12 vs 6), and the projected point preserves that same 2:1 ratio (2.5 vs 1.25) — exactly what we expect, since perspective projection just uniformly scales both in-plane coordinates by the same factor `f/x3` (here, `5/24`).

---

### Solution 3 — Luminance & CMY (Hard)

**Given:**
```
R = 0.80,  G = 0.40,  B = 0.20
```

**Part (a) — Luminance:**
```
Y = 0.2990·R + 0.5870·G + 0.1140·B
Y = 0.2990 × 0.80 + 0.5870 × 0.40 + 0.1140 × 0.20
Y = 0.2392        + 0.2348        + 0.0228
Y = 0.4968
```

**Y ≈ 0.497** (roughly mid-gray — makes sense, since R and G are both moderately-to-fairly bright while B is dim, and green's dominant weight (0.587) pulls the result up despite G being smaller than R).

**Part (b) — CMY conversion:**
```
C     = 1 - R = 1 - 0.80 = 0.20
M     = 1 - G = 1 - 0.40 = 0.60
Y_ink = 1 - B = 1 - 0.20 = 0.80
```

**Final Answer:**
```
Y (luminance) ≈ 0.497
(C, M, Y_ink) = (0.20, 0.60, 0.80)
```

**Interpretation check:** the original color is a strong, warm orange-ish tone (high R, medium G, low B — think terracotta). Converting to ink: since R is high, very little **cyan** ink is needed (C = 0.20, cyan being red's "opposite"/absorber). Since B is low, a *lot* of **yellow** ink is needed (Y_ink = 0.80, yellow absorbing blue). This lines up perfectly with intuition — printing an orange color needs mostly yellow + a moderate amount of magenta, and very little cyan.

---

## 🎓 Recap — The Full Pipeline

```
Real-world light
      │
      ▼
[1] Image Formation — pinhole/lens geometry
    perspective projection (y1, y2) = f·(x1, x2)/x3
      │
      ▼
[2] Image Sensing — CCD/CMOS + Bayer filter
    photons → charge → voltage, per-channel color separation
      │
      ▼
[3] Sampling — finite grid of pixel locations
    (risk: Moiré patterns if undersampled)
      │
      ▼
[4] Quantization — finite bits per intensity value
    (risk: banding/posterization if too coarse)
      │
      ▼
[5] Color Space Engineering
    RGB/CMY, HSV/HSL, YUV/YCbCr + chroma subsampling
      │
      ▼
   Your digital image matrix 🎉
```

Every one of these five stages introduces its own engineering trade-offs — brightness vs. sharpness, sensor count vs. per-pixel signal, sampling rate vs. memory, bit-depth vs. storage, and perceptual detail vs. bandwidth. That's the throughline of the whole lecture: **digital image acquisition is one long chain of "how much are we willing to lose, and where, to save resources?"**

Good luck, and see you next week! 🚀
