#ifndef XRAFT__TEXT_MODEL_H
#define XRAFT__TEXT_MODEL_H

#include <deque>
#include <list>

#include "gapped"

namespace xraft
{

template<typename T_attribute>
class t_text_model
{
public:
	typedef gapped<wchar_t>::const_iterator t_iterator;

	struct t_segment
	{
		size_t v_text;
		T_attribute v_attribute;

		t_segment(size_t a_text = 0, const T_attribute& a_attribute = T_attribute()) : v_text(a_text), v_attribute(a_attribute)
		{
		}
	};
	struct t_observer
	{
		virtual void f_loaded() = 0;
		virtual void f_replacing(size_t a_p, size_t a_n) = 0;
		virtual void f_replaced(size_t a_n) = 0;
	};

private:
	typedef gapped<t_segment> t_segments;

	struct t_convert
	{
		size_t v_size;

		t_convert(size_t a_size) : v_size(a_size)
		{
		}
		t_segment operator()(const t_segment& a_x) const
		{
			return t_segment(v_size - a_x.v_text, a_x.v_attribute);
		}
	};

	gapped<wchar_t> v_text;
	gapped<t_segment> v_segments;
	std::list<t_observer*> v_observers;
	bool v_notify = false;

protected:
	void f_fire_loaded()
	{
		if (v_notify) for (auto p : v_observers) p->f_loaded();
	}
	void f_fire_replacing(size_t a_p, size_t a_n)
	{
		if (v_notify) for (auto p : v_observers) p->f_replacing(a_p, a_n);
	}
	void f_fire_replaced(size_t a_n)
	{
		if (v_notify) for (auto p : v_observers) p->f_replaced(a_n);
	}
	typename t_segments::const_iterator f_text_to_segment_(size_t a_p) const;
	void f_erase_attribute(size_t a_p, size_t a_n);
	void f_insert_attribute(size_t a_p, size_t a_n, const T_attribute& a_attribute);

public:
	void f_add_observer(t_observer* a_observer)
	{
		v_observers.push_back(a_observer);
	}
	void f_remove_observer(t_observer* a_observer)
	{
		v_observers.remove(a_observer);
	}
	void f_enable_notification()
	{
		if (v_notify) return;
		v_notify = true;
		f_fire_loaded();
	}
	void f_disable_notification()
	{
		v_notify = false;
	}
	t_iterator f_begin() const
	{
		return v_text.begin();
	}
	t_iterator f_end() const
	{
		return v_text.end();
	}
	size_t f_text_size() const
	{
		return v_text.size();
	}
	size_t f_segments_size() const
	{
		return v_segments.size();
	}
	size_t f_segment_to_text(size_t a_p) const
	{
		return a_p < v_segments.gap_index() ? v_segments[a_p].v_text : v_text.size() - v_segments[a_p].v_text;
	}
	const T_attribute& f_segment_to_attribute(size_t a_p) const
	{
		return v_segments[a_p].v_attribute;
	}
	size_t f_text_to_segment(size_t a_p) const
	{
		return f_text_to_segment_(a_p) - v_segments.begin();
	}
	template<typename T_segments, typename T_text>
	void f_replace(size_t a_p, size_t a_n, T_segments a_f, T_segments a_l, T_text a_text);
	void f_attribute(size_t a_p, size_t a_n, const T_attribute& a_attribute)
	{
		f_fire_replacing(a_p, a_n);
		f_erase_attribute(a_p, a_n);
		f_insert_attribute(a_p, a_n, a_attribute);
		f_fire_replaced(a_n);
	}
};

template<typename T_attribute>
typename t_text_model<T_attribute>::t_segments::const_iterator t_text_model<T_attribute>::f_text_to_segment_(size_t a_p) const
{
	auto si = v_segments.gap();
	auto send = v_segments.end();
	if (si == send || a_p < v_text.size() - si->v_text) {
		auto sbegin = v_segments.begin();
		while (si != sbegin) if ((--si)->v_text <= a_p) break;
	} else {
		do {
			if (++si == send) {
				if (a_p < v_text.size()) --si;
				return si;
			}
		} while (v_text.size() - si->v_text <= a_p);
		--si;
	}
	return si;
}

template<typename T_attribute>
void t_text_model<T_attribute>::f_erase_attribute(size_t a_p, size_t a_n)
{
	auto si = v_segments.gap();
	auto send = v_segments.end();
	if (a_p < (si == send ? v_text.size() : v_text.size() - si->v_text)) {
		do --si; while (si->v_text > a_p);
		if (si->v_text < a_p) ++si;
		v_segments.move_backward(v_segments.gap() - si, t_convert(v_text.size()));
	} else {
		while (si != send && v_text.size() - si->v_text < a_p) ++si;
		v_segments.move_forward(si - v_segments.gap(), t_convert(v_text.size()));
	}
	si = v_segments.gap();
	while (true) {
		if (si == send) {
			if (a_p + a_n < v_text.size()) --si;
			break;
		} else if (v_text.size() - si->v_text > a_p + a_n) {
			--si;
			break;
		}
		++si;
	}
	if (si < v_segments.gap()) return;
	if (si != send) {
		auto sj = v_segments.gap();
		if (sj != v_segments.begin() && (--sj)->v_attribute == si->v_attribute) {
			v_segments.erase_forward(++si - v_segments.gap());
			return;
		}
		const_cast<size_t&>(si->v_text) = v_text.size() - a_p - a_n;
	}
	v_segments.erase_forward(si - v_segments.gap());
}

template<typename T_attribute>
void t_text_model<T_attribute>::f_insert_attribute(size_t a_p, size_t a_n, const T_attribute& a_attribute)
{
	if (a_n <= 0) return;
	auto si = v_segments.gap();
	if (si == v_segments.begin()) {
		if (si != v_segments.end() && a_attribute == si->v_attribute) v_segments.erase_forward(1);
		v_segments.insert_forward(t_segment(0, a_attribute));
	} else if (a_p + a_n < (si == v_segments.end() ? v_text.size() : v_text.size() - si->v_text)) {
		if (a_attribute != (--si)->v_attribute) {
			v_segments.insert_forward(t_segment(a_p, a_attribute));
			v_segments.insert_backward(t_segment(v_text.size() - a_p - a_n, si->v_attribute));
		}
	} else if (a_attribute != (--si)->v_attribute) {
		si = v_segments.gap();
		if (si != v_segments.end() && a_attribute == si->v_attribute) v_segments.erase_forward(1);
		v_segments.insert_forward(t_segment(a_p, a_attribute));
	}
}

template<typename T_attribute>
template<typename T_segments, typename T_text>
void t_text_model<T_attribute>::f_replace(size_t a_p, size_t a_n, T_segments a_f, T_segments a_l, T_text a_text)
{
	f_fire_replacing(a_p, a_n);
	f_erase_attribute(a_p, a_n);
	size_t i = v_text.gap_index();
	if (a_p > i)
		v_text.move_forward(a_p - i);
	else
		v_text.move_backward(i - a_p);
	v_text.erase_forward(a_n);
	size_t n = 0;
	while (a_f != a_l) {
		v_text.insert_forward(a_text, a_text + a_f->v_text);
		f_insert_attribute(a_p, a_f->v_text, a_f->v_attribute);
		a_p += a_f->v_text;
		a_text += a_f->v_text;
		n += a_f->v_text;
		++a_f;
	}
	f_fire_replaced(n);
}

}

#endif
