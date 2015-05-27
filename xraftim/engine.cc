#include "engine.h"

#include <algorithm>
#include <cctype>

#include "roman.wide"

const t_engine::t_action* t_engine::t_map::f_find(char a_key) const
{
	const t_action* const* i = std::lower_bound(v_first, v_last, a_key, t_less());
	return i == v_last || (*i)->v_key != a_key ? 0 : *i;
}

void t_engine::t_action_push::f_key_pressed(t_engine* a_engine) const
{
	a_engine->f_roman_push(v_key, &v_next);
}

void t_engine::t_action_reduce::f_key_pressed(t_engine* a_engine) const
{
	if (a_engine->v_katakana)
		a_engine->f_roman_reduce(v_cs1, std::wcslen(v_cs1));
	else
		a_engine->f_roman_reduce(v_cs0, std::wcslen(v_cs0));
	if (v_next)
		v_next->f_key_pressed(a_engine);
	else
		a_engine->f_roman_commit();
}

void t_engine::f_roman_reset()
{
	t_state& state = v_states.back();
	size_t i = state.v_text + state.v_entry;
	size_t n = v_buffer.size() - state.v_base - i;
	if (state.v_okuri > 0) {
		--n;
		state.v_okuri = 0;
	}
	v_buffer.erase(v_buffer.begin() + state.v_base + i, v_buffer.end());
	v_map = &t_roman_table::v_default;
	f_on_compose(i, n, 0, 0, 0);
}

void t_engine::f_roman_push(wchar_t a_c, const t_map* a_map)
{
	t_state& state = v_states.back();
	size_t i = v_buffer.size() - state.v_base;
	if (state.v_okuri > 0) --i;
	v_buffer.push_back(a_c);
	v_map = a_map;
	t_attribute a = e_attribute__ROMAN;
	f_on_compose(i, 0, &a_c, &a, 1);
}

void t_engine::f_roman_reduce(const wchar_t* a_cs, size_t a_n)
{
	t_state& state = v_states.back();
	size_t i = state.v_text + state.v_entry + state.v_okuri;
	size_t n = v_buffer.size() - state.v_base - i;
	v_buffer.erase(v_buffer.end() - n, v_buffer.end());
	v_buffer.insert(v_buffer.end(), a_cs, a_cs + a_n);
	if (v_mode == &t_engine::f_mode_kanji) {
		if (state.v_okuri > 0) {
			state.v_okuri += a_n;
			std::vector<t_attribute> as(a_n, e_attribute__OKURI);
			f_on_compose(--i, n, a_cs, &as[0], a_n);
		} else {
			state.v_entry += a_n;
			std::vector<t_attribute> as(a_n, e_attribute__ENTRY);
			f_on_compose(i, n, a_cs, &as[0], a_n);
		}
	} else if (v_states.size() > 1) {
		state.v_text += a_n;
		std::vector<t_attribute> as(a_n, e_attribute__TEXT);
		f_on_compose(i, n, a_cs, &as[0], a_n);
	} else {
		state = t_state();
		f_on_commit(&v_buffer[0], v_buffer.size());
		v_buffer.clear();
	}
}

void t_engine::f_roman_commit()
{
	if (v_states.back().v_okuri > 0) f_choose();
	v_map = &t_roman_table::v_default;
}

void t_engine::f_text_push(char a_ascii)
{
	wchar_t c = a_ascii;
	if (v_states.size() > 1) {
		t_state& state = v_states.back();
		size_t i = state.v_text;
		v_buffer.push_back(c);
		++state.v_text;
		t_attribute a = e_attribute__TEXT;
		f_on_compose(i, 0, &c, &a, 1);
	} else {
//		f_on_forward();
		f_on_commit(&c, 1);
	}
}

void t_engine::f_text_pop()
{
	if (v_states.back().v_text > 0) {
		t_state& state = v_states.back();
		v_buffer.pop_back();
		--state.v_text;
		f_on_compose(state.v_text, 1, 0, 0, 0);
	} else if (v_states.size() > 1) {
		v_states.pop_back();
		t_state& state = v_states.back();
		v_buffer.erase(v_buffer.begin() + state.v_base + state.v_text + state.v_entry, v_buffer.end());
		state.v_okuri = 0;
		v_mode = &t_engine::f_mode_kanji;
		std::vector<t_attribute> as;
		as.insert(as.end(), state.v_text, e_attribute__TEXT);
		as.insert(as.end(), state.v_entry, e_attribute__ENTRY);
		f_on_compose(0, 0, &v_buffer[state.v_base], &as[0], as.size());
		f_on_status();
	} else {
		f_on_forward();
	}
}

void t_engine::f_text_commit()
{
	if (v_states.size() > 1) {
		size_t base = v_states.back().v_base;
		size_t text = v_states.back().v_text;
		v_states.pop_back();
		t_state& state = v_states.back();
		auto i = v_buffer.begin() + state.v_base + state.v_text;
		if (text > 0) v_dictionary.f_register(&*i, state.v_entry, state.v_okuri, &v_buffer[base], text);
		auto j = i + state.v_entry;
		if (state.v_okuri > 0) {
			++j;
			--state.v_okuri;
		}
		i = v_buffer.erase(i, j);
		std::rotate(i, i + state.v_okuri, v_buffer.end());
		v_mode = &t_engine::f_mode_roman;
		if (v_states.size() > 1) {
			state.v_text = v_buffer.size() - state.v_base;
			state.v_entry = state.v_okuri = 0;
			std::vector<t_attribute> as;
			as.insert(as.end(), state.v_text, e_attribute__TEXT);
			f_on_compose(0, text, &v_buffer[state.v_base], &as[0], as.size());
		} else {
			state = t_state();
			f_on_commit(&v_buffer[0], v_buffer.size());
			v_buffer.clear();
		}
		f_on_status();
	} else {
		f_on_forward();
	}
}

void t_engine::f_entry_push(char a_ascii)
{
	t_state& state = v_states.back();
	size_t i = state.v_text + state.v_entry;
	wchar_t c = a_ascii;
	v_buffer.push_back(c);
	++state.v_entry;
	t_attribute a = e_attribute__ENTRY;
	f_on_compose(i, 0, &c, &a, 1);
}

void t_engine::f_entry_pop()
{
	t_state& state = v_states.back();
	if (state.v_entry > 0) {
		v_buffer.pop_back();
		--state.v_entry;
		f_on_compose(state.v_text + state.v_entry, 1, 0, 0, 0);
	}
	if (state.v_entry <= 0) {
		v_mode = &t_engine::f_mode_roman;
		f_on_status();
	}
}

void t_engine::f_entry_commit()
{
	t_state& state = v_states.back();
	v_mode = &t_engine::f_mode_roman;
	if (v_states.size() > 1) {
		size_t i = state.v_text;
		size_t n = state.v_entry;
		state.v_text += n;
		state.v_entry = state.v_okuri = 0;
		std::vector<t_attribute> as;
		as.insert(as.end(), n, e_attribute__TEXT);
		f_on_compose(i, n, &v_buffer[state.v_base + i], &as[0], n);
	} else {
		state = t_state();
		f_on_commit(&v_buffer[0], v_buffer.size());
		v_buffer.clear();
	}
	f_on_status();
}

void t_engine::f_choose()
{
	t_state& state = v_states.back();
	v_dictionary.f_search(&v_buffer[state.v_base + state.v_text], state.v_entry, state.v_okuri, v_candidates);
	if (v_candidates.empty()) {
		size_t n = state.v_text + state.v_entry;
		if (state.v_okuri > 0) n += state.v_okuri - 1;
		v_mode = &t_engine::f_mode_roman;
		v_states.push_back(t_state(v_buffer.size()));
		f_on_compose(0, n, 0, 0, 0);
	} else {
		v_mode = &t_engine::f_mode_choose;
		v_chosen = 0;
		v_choosing = false;
		size_t i = state.v_text;
		size_t n = state.v_entry;
		const std::wstring& s = v_candidates[0].v_text;
		std::vector<t_attribute> as;
		as.insert(as.end(), s.size(), e_attribute__CANDIDATE);
		f_on_compose(i, n, s.c_str(), &as[0], s.size());
	}
	f_on_status();
}

void t_engine::f_mode_latin(t_modifier a_modifier, t_key a_key, char a_ascii)
{
	if (a_key == e_key__RETURN || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__M) {
		f_text_commit();
	} else if (a_key == e_key__LINEFEED || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__J) {
		v_mode = &t_engine::f_mode_roman;
		f_on_status();
	} else if (a_key == e_key__BACK_SPACE || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__H) {
		f_text_pop();
	} else if (std::isprint(a_ascii)) {
		f_text_push(a_ascii);
	} else {
		if (v_buffer.empty()) f_on_forward();
	}
}

void t_engine::f_mode_word(t_modifier a_modifier, t_key a_key, char a_ascii)
{
	if (a_key == e_key__RETURN || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__M) {
		f_entry_commit();
	} else if (a_key == e_key__LINEFEED || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__J) {
		if (v_states.back().v_entry > 0)
			v_mode = &t_engine::f_mode_kanji;
		else
			v_mode = &t_engine::f_mode_roman;
		f_on_status();
	} else if (a_key == e_key__BACK_SPACE || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__H) {
		f_entry_pop();
	} else if (a_key == e_key__SPACE) {
		if (v_states.back().v_entry > 0) f_choose();
	} else if (std::isprint(a_ascii)) {
		f_entry_push(a_ascii);
	}
}

void t_engine::f_mode_roman(t_modifier a_modifier, t_key a_key, char a_ascii)
{
	if (a_key == e_key__RETURN || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__M) {
		if (v_map != &t_roman_table::v_default) f_roman_reset();
		f_text_commit();
	} else if (a_key == e_key__BACK_SPACE || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__H) {
		if (v_map != &t_roman_table::v_default)
			f_roman_reset();
		else
			f_text_pop();
	} else if (a_key == e_key__LINEFEED || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__J) {
		if (v_map != &t_roman_table::v_default) f_roman_reset();
	} else if ((a_modifier & e_modifier__CONTROL) != 0) {
		if (v_map != &t_roman_table::v_default) f_roman_reset();
		if (v_buffer.empty()) f_on_forward();
	} else if (std::isprint(a_ascii)) {
		char key = std::tolower(a_ascii);
		const t_action* action = v_map->f_find(key);
		if (!action && v_map != &t_roman_table::v_default) {
			f_roman_reset();
			action = v_map->f_find(key);
		}
		if (action) {
			if (v_map == &t_roman_table::v_default && key != a_ascii) {
				v_mode = &t_engine::f_mode_kanji;
				f_on_status();
			}
			action->f_key_pressed(this);
		} else {
			switch (a_ascii) {
			case '/':
				v_mode = &t_engine::f_mode_word;
				f_on_status();
				break;
			case 'l':
				v_mode = &t_engine::f_mode_latin;
				f_on_status();
				break;
			case 'q':
				v_katakana ^= true;
				f_on_status();
				break;
			default:
				f_text_push(a_ascii);
			}
		}
	} else {
		if (v_buffer.empty()) f_on_forward();
	}
}

void t_engine::f_mode_kanji(t_modifier a_modifier, t_key a_key, char a_ascii)
{
	if (a_key == e_key__RETURN || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__M) {
		if (v_map != &t_roman_table::v_default) f_roman_reset();
		f_entry_commit();
	} else if (a_key == e_key__BACK_SPACE || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__H) {
		if (v_map != &t_roman_table::v_default) {
			f_roman_reset();
			if (v_buffer.empty()) {
				v_mode = &t_engine::f_mode_roman;
				f_on_status();
			}
		} else {
			f_entry_pop();
		}
	} else if ((a_modifier & e_modifier__CONTROL) != 0) {
		if (v_map != &t_roman_table::v_default) f_roman_reset();
	} else if (std::isprint(a_ascii)) {
		char key = std::tolower(a_ascii);
		const t_action* action = v_map->f_find(key);
		if (!action && v_map != &t_roman_table::v_default) {
			f_roman_reset();
			action = v_map->f_find(a_ascii);
		}
		if (action) {
			t_state& state = v_states.back();
			if (v_map == &t_roman_table::v_default && key != a_ascii && state.v_entry > 0) {
				v_buffer.push_back(key);
				state.v_okuri = 1;
			}
			action->f_key_pressed(this);
		} else {
			switch (a_ascii) {
			case ' ':
				f_choose();
				break;
			case '/':
			case 'l':
				v_mode = &t_engine::f_mode_word;
				f_on_status();
				break;
			case 'q':
				v_katakana ^= true;
				f_on_status();
				break;
			default:
				f_entry_push(a_ascii);
			}
		}
	}
}

void t_engine::f_mode_choose(t_modifier a_modifier, t_key a_key, char a_ascii)
{
	t_state& state = v_states.back();
	size_t n = v_candidates[v_chosen].v_text.size();
	if (a_key == e_key__BACK_SPACE || (a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__H || a_key == e_key__X && v_chosen <= 0) {
		if (state.v_okuri > 0) n += state.v_okuri - 1;
		v_mode = &t_engine::f_mode_kanji;
		v_buffer.erase(v_buffer.begin() + state.v_base + state.v_text + state.v_entry, v_buffer.end());
		state.v_okuri = 0;
		v_candidates.clear();
		std::vector<t_attribute> as;
		as.insert(as.end(), state.v_entry, e_attribute__ENTRY);
		f_on_compose(state.v_text, n, &v_buffer[state.v_base + state.v_text], &as[0], as.size());
		f_on_status();
		f_on_candidates();
	} else if (a_key == e_key__SPACE) {
		if (++v_chosen < v_candidates.size()) {
			const std::wstring& s = v_candidates[v_chosen].v_text;
			std::vector<t_attribute> as;
			as.insert(as.end(), s.size(), e_attribute__CANDIDATE);
			f_on_compose(state.v_text, n, s.c_str(), &as[0], s.size());
			if (v_choosing) {
				f_on_choose();
			} else {
				v_choosing = true;
				f_on_candidates();
			}
		} else {
			n += state.v_text;
			if (state.v_okuri > 0) n += state.v_okuri - 1;
			v_mode = &t_engine::f_mode_roman;
			v_states.push_back(t_state(v_buffer.size()));
			v_candidates.clear();
			f_on_compose(0, n, 0, 0, 0);
			f_on_status();
			f_on_candidates();
		}
	} else if (a_key == e_key__X) {
		--v_chosen;
		const std::wstring& s = v_candidates[v_chosen].v_text;
		std::vector<t_attribute> as;
		as.insert(as.end(), s.size(), e_attribute__CANDIDATE);
		f_on_compose(state.v_text, n, s.c_str(), &as[0], s.size());
		f_on_choose();
	} else if (a_key == e_key__RETURN || std::isprint(a_ascii)) {
		auto i = v_buffer.begin() + state.v_base + state.v_text;
		const std::wstring& s = v_candidates[v_chosen].v_text;
		v_dictionary.f_register(&*i, state.v_entry, state.v_okuri, s.c_str(), s.size());
		auto j = i + state.v_entry;
		if (state.v_okuri > 0) ++j;
		i = v_buffer.erase(i, j);
		v_buffer.insert(i, s.begin(), s.end());
		v_mode = &t_engine::f_mode_roman;
		v_candidates.clear();
		if (v_states.size() > 1) {
			if (state.v_okuri > 0) n += state.v_okuri - 1;
			size_t i = state.v_text;
			state.v_text = v_buffer.size() - state.v_base;
			state.v_entry = state.v_okuri = 0;
			std::vector<t_attribute> as;
			as.insert(as.end(), state.v_text - i, e_attribute__TEXT);
			f_on_compose(i, n, &v_buffer[state.v_base + i], &as[0], as.size());
		} else {
			state = t_state();
			f_on_commit(&v_buffer[0], v_buffer.size());
			v_buffer.clear();
		}
		f_on_status();
		f_on_candidates();
		if (a_key == e_key__RETURN) return;
		if ((a_modifier & e_modifier__CONTROL) != 0 && a_key == e_key__M) return;
		f_key_pressed(a_modifier, a_key, a_ascii);
	}
}

t_engine::t_engine(t_dictionary& a_dictionary) : v_dictionary(a_dictionary), v_map(&t_roman_table::v_default), v_states(1)
{
}

std::wstring t_engine::f_status() const
{
	if (v_mode == &t_engine::f_mode_latin) return L"Aa";
	if (v_mode == &t_engine::f_mode_word) return L"!Aa";
	if (v_mode == &t_engine::f_mode_kanji) return v_katakana ? L"!\u30a2" : L"!\u3042";
	if (v_mode == &t_engine::f_mode_choose) return L'?' + f_entry(v_states.size() - 1);
	return v_katakana ? L"\u30a2" : L"\u3042";
}

std::wstring t_engine::f_entry(size_t a_i) const
{
	const t_state& state = v_states[a_i];
	const wchar_t* p = &v_buffer[state.v_base + state.v_text];
	std::vector<wchar_t> cs(p, p + state.v_entry);
	if (state.v_okuri > 0) {
		cs.push_back(L' ');
		p += state.v_entry;
		cs.insert(cs.end(), p + 1, p + state.v_okuri);
	}
	return std::wstring(cs.begin(), cs.end());
}

void t_engine::f_reset()
{
	t_state& state = v_states.back();
	size_t n = v_buffer.size() - state.v_base;
	if (state.v_okuri > 0) --n;
	if (!v_candidates.empty()) {
		n -= state.v_entry;
		n += v_candidates[v_chosen].v_text.size();
	}
	v_mode = &t_engine::f_mode_roman;
	v_buffer.clear();
	v_katakana = false;
	v_map = &t_roman_table::v_default;
	v_states.clear();
	v_states.push_back(t_state());
	v_candidates.clear();
	f_on_compose(0, n, 0, 0, 0);
	f_on_status();
	f_on_candidates();
}
