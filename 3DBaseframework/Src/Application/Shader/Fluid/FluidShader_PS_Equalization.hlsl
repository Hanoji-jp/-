// ===================================================
// Anti-diffusion pass (mass sharpening / re-concentration).
//  The advection's bilinear scatter blurs the water/air interface every frame,
//  so the mass (z) of a settled pool slowly diffuses upward and the measured
//  surface drifts down. This pass reverses that blur with a conservative
//  "negative Laplacian" on z: mass flows from lower-density cells toward
//  higher-density neighbours (opposite of diffusion), sharpening the interface
//  while conserving total mass (paired fluxes cancel; walls = zero flux).
//  Total is also pinned by the separate mass-normalization pass.
// ===================================================

Texture2D<float4> g_quantity : register(t0);
Texture2D<float4> g_solid    : register(t1);	// r: 1 = inside cup / 0 = wall

cbuffer cbEqualization : register(b0)
{
	float  g_surplusRate;	// unused here
	float  g_deficitRate;	// anti-diffusion strength (per step)
	float2 _pad;
	float4 g_deficitRatio;	// unused here
};

float4 main(float4 svPos : SV_Position) : SV_Target
{
	uint W, H;
	g_quantity.GetDimensions(W, H);

	int2   c = int2(svPos.xy);
	float4 q = g_quantity.Load(int3(c, 0));

	// Walls are left untouched.
	if (g_solid.Load(int3(c, 0)).r < 0.5) { return q; }

	const float zc = q.z;

	// Sum of (neighbour z - zc) over the 4-neighbourhood.
	//  A wall neighbour is treated as = zc (zero flux at the boundary),
	//  which keeps the operator mass-conserving inside the cup.
	int2 nb[4] =
	{
		int2(c.x - 1, c.y), int2(c.x + 1, c.y),
		int2(c.x, c.y - 1), int2(c.x, c.y + 1),
	};

	float lap = 0.0;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		int2 p = clamp(nb[i], int2(0, 0), int2((int)W - 1, (int)H - 1));
		float solidN = g_solid.Load(int3(p, 0)).r;
		float zn = (solidN < 0.5) ? zc : g_quantity.Load(int3(p, 0)).z;
		lap += (zn - zc);
	}

	// Anti-diffusion: move z against the Laplacian (sharpen). Conservative.
	q.z = max(zc - g_deficitRate * lap, 0.0);
	return q;
}
