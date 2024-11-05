#include <xraft/input_context.h>

namespace xraft
{

t_input_context::~t_input_context()
{
	ImmDestroyContext(v_himc);
}

void t_input_context::f_composition(std::vector<wchar_t>& a_cs, std::vector<t_input_attribute>& a_as)
{
	LONG n = ImmGetCompositionString(v_himc, GCS_COMPSTR, NULL, 0) / sizeof(wchar_t);
	a_cs.resize(n);
	ImmGetCompositionString(v_himc, GCS_COMPSTR, &a_cs[0], n * sizeof(wchar_t));
	n = ImmGetCompositionString(v_himc, GCS_COMPATTR, NULL, 0);
	unsigned char* as = new unsigned char[n];
	ImmGetCompositionString(v_himc, GCS_COMPATTR, as, n);
	a_as.resize(n);
	for (LONG i = 0; i < n; ++i) {
		switch (as[i] & 0x0f) {
		case ATTR_TARGET_CONVERTED:
		case ATTR_TARGET_NOTCONVERTED:
			a_as[i] = static_cast<t_input_attribute>(c_input_attribute__REVERSE | c_input_attribute__UNDERLINE);
			break;
		default:
			a_as[i] = c_input_attribute__UNDERLINE;
		}
	}
	delete[] as;
}

}
