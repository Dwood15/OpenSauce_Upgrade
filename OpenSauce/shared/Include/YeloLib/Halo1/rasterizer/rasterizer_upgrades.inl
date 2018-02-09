/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/

class c_render_upgrades
{
public:
	// If we're not compiling with any upgrades, then don't waste space in the compiled code
	uint32 rendered_triangles[
							Enums::k_maximum_rendered_triangles_upgrade <= Enums::k_maximum_rendered_triangles ? 1 :
							Enums::k_maximum_rendered_triangles_upgrade];

	void InitializeRenderedTrianglesUpgrade()
	{
		if (NUMBEROF(rendered_triangles) > 1)
		{
			// redirect all rendered triangle pointers to the new array
			for (auto ptr : K_RENDERED_TRIANGLE_ARRAY_UPGRADE_ADDRESS_LIST)
				*ptr = rendered_triangles;
			// change all references to the rendered triangle array to our new size
			for (auto ptr : K_MAXIMUM_RENDERED_TRIANGLES_UPGRADE_ADDRESS_LIST)
				*ptr = Enums::k_maximum_rendered_triangles_upgrade;
		}
	}
};
static c_render_upgrades g_render_upgrades;