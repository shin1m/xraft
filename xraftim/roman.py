# coding: utf-8

import re

source = ur'''
, 、 、
- ー ー
. 。 。
a ア あ
b {
	a バ ば
	b ッ っ b
	e ベ べ
	i ビ び
	o ボ ぼ
	u ブ ぶ
	y {
		a ビャ びゃ
		e ビェ びぇ
		i ビィ びぃ
		o ビョ びょ
		u ビュ びゅ
	}
}
c {
	c ッ っ c
	h {
		a チャ ちゃ
		e チェ ちぇ
		i チ ち
		o チョ ちょ
		u チュ ちゅ
	}
	y {
		a チャ ちゃ
		e チェ ちぇ
		i チィ ちぃ
		o チョ ちょ
		u チュ ちゅ
	}
}
d {
	a ダ だ
	d ッ っ d
	e デ で
	h {
		a デャ でゃ
		e デェ でぇ
		i ディ でぃ
		o デョ でょ
		u デュ でゅ
	}
	i ヂ ぢ
	o ド ど
	u ヅ づ
	w {
		a ドァ どぁ
		e ドェ どぇ
		i ドィ どぃ
		o ドォ どぉ
		u ドゥ どぅ
	}
	y {
		a ヂャ ぢゃ
		e ヂェ ぢぇ
		i ヂィ ぢぃ
		o ヂョ ぢょ
		u ヂュ ぢゅ
	}
}
e エ え
f {
	a ファ ふぁ
	e フェ ふぇ
	f ッ っ f
	i フィ ふぃ
	o フォ ふぉ
	u フ ふ
	y {
		a フャ ふゃ
		e フェ ふぇ
		i フィ ふぃ
		o フョ ふょ
		u フュ ふゅ
	}
}
g {
	a ガ が
	e ゲ げ
	g ッ っ g
	i ギ ぎ
	o ゴ ご
	u グ ぐ
	y {
		a ギャ ぎゃ
		e ギェ ぎぇ
		i ギィ ぎぃ
		o ギョ ぎょ
		u ギュ ぎゅ
	}
}
h {
	a ハ は
	e ヘ へ
	h ッ っ h
	i ヒ ひ
	o ホ ほ
	u フ ふ
	y {
		a ヒャ ひゃ
		e ヒェ ひぇ
		i ヒィ ひぃ
		o ヒョ ひょ
		u ヒュ ひゅ
	}
}
i イ い
j {
	a ジャ じゃ
	e ジェ じぇ
	i ジ じ
	j ッ っ j
	o ジョ じょ
	u ジュ じゅ
	y {
		a ジャ じゃ
		e ジェ じぇ
		i ジィ じぃ
		o ジョ じょ
		u ジュ じゅ
	}
}
k {
	a カ か
	e ケ け
	i キ き
	k ッ っ k
	o コ こ
	u ク く
	y {
		a キャ きゃ
		e キェ きぇ
		i キィ きぃ
		o キョ きょ
		u キュ きゅ
	}
}
m {
	a マ ま
	e メ め
	i ミ み
	o モ も
	u ム む
	y {
		a ミャ みゃ
		e ミェ みぇ
		i ミィ みぃ
		o ミョ みょ
		u ミュ みゅ
	}
}
n {
	a ナ な
	e ネ ね
	i ニ に
	n ン ん
	o ノ の
	u ヌ ぬ
	y {
		a ニャ にゃ
		e ニェ にぇ
		i ニィ にぃ
		o ニョ にょ
		u ニュ にゅ
	}
}
o オ お
p {
	a パ ぱ
	e ペ ぺ
	i ピ ぴ
	o ポ ぽ
	p ッ っ p
	u プ ぷ
	y {
		a ピャ ぴゃ
		e ピェ ぴぇ
		i ピィ ぴぃ
		o ピョ ぴょ
		u ピュ ぴゅ
	}
}
r {
	a ラ ら
	e レ れ
	i リ り
	o ロ ろ
	r ッ っ r
	u ル る
	y {
		a リャ りゃ
		e リェ りぇ
		i リィ りぃ
		o リョ りょ
		u リュ りゅ
	}
}
s {
	a サ さ
	e セ せ
	h {
		a シャ しゃ
		e シェ しぇ
		i シ し
		o ショ しょ
		u シュ しゅ
	}
	i シ し
	o ソ そ
	s ッ っ s
	u ス す
	y {
		a シャ しゃ
		e シェ しぇ
		i シィ しぃ
		o ショ しょ
		u シュ しゅ
	}
}
t {
	a タ た
	e テ て
	h {
		a テァ てぁ
		e テェ てぇ
		i ティ てぃ
		o テョ てょ
		u テュ てゅ
	}
	i チ ち
	o ト と
	s {
		u ツ つ
	}
	t ッ っ t
	u ツ つ
	y {
		a チャ ちゃ
		e チェ ちぇ
		i チィ ちぃ
		o チョ ちょ
		u チュ ちゅ
	}
}
u ウ う
v {
	a ヴァ う゛ぁ
	e ヴェ う゛ぇ
	i ヴィ う゛ぃ
	o ヴォ う゛ぉ
	u ヴ う゛
	v ッ っ v
}
w {
	a ワ わ
	e ウェ うぇ
	h {
		a ウァ うぁ
		e ウェ うぇ
		i ウィ うぃ
		o ウォ うぉ
		u ウゥ うぅ
	}
	i ウィ うぃ
	o ヲ を
	u ウ う
	w ッ っ w
}
x {
	, ， ，
	. ． ．
	/ ・ ・
	a ァ ぁ
	e ェ ぇ
	i ィ ぃ
	k {
		a ヵ か
		e ヶ け
	}
	o ォ ぉ
	t {
		s {
			u ッ っ
		}
		u ッ っ
	}
	u ゥ ぅ
	w {
		a ヮ ゎ
		e ヱ ゑ
		i ヰ ゐ
	}
	x ッ っ x
	y {
		a ャ ゃ
		o ョ ょ
		u ュ ゅ
	}
}
y {
	a ヤ や
	e イェ いぇ
	o ヨ よ
	u ユ ゆ
	y ッ っ y
}
z {
	! ！ ！
	" ″ ″
	# ＃ ＃
	$ ＄ ＄
	% ％ ％
	& ＆ ＆
	' ′ ′
	( （ （
	) ） ）
	* ＊ ＊
	+ ＋ ＋
	, , ,
	- - -
	. . .
	/ ／ ／
	0 ０ ０
	1 １ １
	2 ２ ２
	3 ３ ３
	4 ４ ４
	5 ５ ５
	6 ６ ６
	7 ７ ７
	8 ８ ８
	9 ９ ９
	: ： ：
	; ； ；
	< ＜ ＜
	= ＝ ＝
	> ＞ ＞
	? ？ ？
	@ ＠ ＠
	[ 「 「
	\ ＼ ＼
	] 」 」
	^ ∧ ∧
	_ ＿ ＿
	` ‘ ‘
	a ザ ざ
	e ゼ ぜ
	i ジ じ
	o ゾ ぞ
	u ズ ず
	y {
		a ジャ じゃ
		e ジェ じぇ
		i ジィ じぃ
		o ジョ じょ
		u ジュ じゅ
	}
	z ッ っ z
	{ ｛ ｛
	| ｜ ｜
	} ｝ ｝
	~ ～ ～
}
'''.split(u'\n')

def f_write(a_value):
	print a_value[1:]

TOKEN_EOF = 0
TOKEN_REDUCE = 1
TOKEN_PUSH_BEGIN = 2
TOKEN_PUSH_END = 3

patterns = [
	re.compile(r'\s*(\S+)\s*(\S+)\s*(\S+)\s*(\S*)'),
	re.compile(r'\s*(\S+)\s*\{'),
	re.compile(r'\s*}')
]

def f_next_token():
	global token, key, katakana, hiragana, next
	while True:
		if not source:
			token = TOKEN_EOF
			return
		s = source.pop(0)
		if s:
			break
	m = patterns[0].match(s)
	if m:
		token = TOKEN_REDUCE
		key = m.group(1)
		katakana = m.group(2)
		hiragana = m.group(3)
		next = m.group(4)
		return
	m = patterns[1].match(s)
	if m:
		token = TOKEN_PUSH_BEGIN
		key = m.group(1)
		return
	if patterns[2].match(s):
		token = TOKEN_PUSH_END
		return
	raise 'invalid line: %s\n' % s

def f_escape(a_c):
	if a_c in (u'\'', u'\\'):
		return '\\' + a_c
	c = ord(a_c)
	if c < 0xa0:# and c not in (0x24, 0x40, 0x60):
		return a_c
	else:
		return r'\u%04x' % c

def f_parse_reduce(a_prefix):
	as = {'prefix': a_prefix, 'variable': '%02x' % ord(key), 'key': f_escape(key)}
	f_write(r'''
	static const t_engine::t_action_reduce %(prefix)s_%(variable)s;
''' % as)
	as['hex0'] = ''.join([f_escape(c) for c in hiragana])
	as['hex1'] = ''.join([f_escape(c) for c in katakana])
	if next:
		as['next'] = ', &v_%02x' % ord(next)
	else:
		as['next'] = ''
	definition = r'''
const t_engine::t_action_reduce t_roman_table::%(prefix)s_%(variable)s('%(key)s', L"%(hex0)s", L"%(hex1)s"%(next)s);
''' % as
	f_next_token()
	return definition

def f_parse_push(a_prefix):
	as = {'prefix': a_prefix, 'pushed': '%02x' % ord(key), 'key': f_escape(key)}
	as['array'] = '%(prefix)s_%(pushed)s_children' % as
	f_write(r'''
	static const t_engine::t_action_push %(prefix)s_%(pushed)s;
	static const t_engine::t_action* const %(array)s[];
''' % as)
	f_next_token()
	children = []
	definitions = []
	while token is not TOKEN_PUSH_END:
		as['variable'] = '%02x' % ord(key)
		children.append(r'&t_roman_table::%(prefix)s_%(pushed)s_%(variable)s' % as)
		if token is TOKEN_REDUCE:
			definitions.append(f_parse_reduce(r'%(prefix)s_%(pushed)s' % as))
		elif token is TOKEN_PUSH_BEGIN:
			definitions.append(f_parse_push(r'%(prefix)s_%(pushed)s' % as))
		else:
			raise 'invalid token\n'
	f_next_token()
	as['size'] = len(children)
	as['children'] = ',\n\t'.join(children)
	as['definitions'] = ''.join(definitions)
	return r'''
const t_engine::t_action_push t_roman_table::%(prefix)s_%(pushed)s('%(key)s', %(array)s, %(array)s + %(size)d);
const t_engine::t_action* const t_roman_table::%(array)s[] = {
	%(children)s
};
%(definitions)s
''' % as

f_next_token()
children = []
definitions = []
f_write(r'''
struct t_roman_table
{
	static const t_engine::t_map v_default;
	static const t_engine::t_action* const v_default_children[];
''')
while token is not TOKEN_EOF:
	children.append('&t_roman_table::v_%02x' % ord(key))
	if token is TOKEN_REDUCE:
		definitions.append(f_parse_reduce('v'))
	elif token is TOKEN_PUSH_BEGIN:
		definitions.append(f_parse_push('v'))
	else:
		raise 'invalid token\n'
as = {
	'size': len(children),
	'children': ',\n\t'.join(children),
	'definitions': ''.join(definitions)
}
f_write(r'''
};

const t_engine::t_map t_roman_table::v_default(t_roman_table::v_default_children, t_roman_table::v_default_children + %(size)d);
const t_engine::t_action* const t_roman_table::v_default_children[] = {
	%(children)s
};
%(definitions)s
''' % as)
