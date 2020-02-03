/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 4;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char barfont[]       = "Cascadia code:size=13";
static const char termfont[]       = "DejaVu Sans Mono:size=12";
static const char *fonts[]          = { barfont };
static const char col_gray0[]       = "#111111";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_blue[]        = "#4C9EFF";
static const char col_yellow[]      = "#FFDB00";
static const char col_red[]         = "#FF420E";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray1, col_blue,   col_blue },
	[SchemeSel]  = { col_gray0, col_yellow, col_red },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ KeyPress,   MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ KeyPress,   MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ KeyPress,   MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ KeyPress,   MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", barfont, "-nb", col_gray1, "-nf", col_gray4, "-sb", col_yellow, "-sf", col_gray1, NULL };
static const char *termcmd[]  = { "st", "-f", termfont, NULL };
static const char *termcmd2[]  = { "st", "-f", termfont, "tmux", NULL };
static const char *termcmd2_snm[]  = { "xterm.sh", "-e", "ssh snm", NULL };

void movestack(const Arg *arg);

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ KeyPress,    MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ KeyPress,    MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ KeyPress,    MODKEY,                       XK_F1,     spawn,          {.v = termcmd2 } },
	{ KeyPress,    MODKEY,                       XK_F2,     spawn,          {.v = termcmd2_snm } },
	{ KeyPress,    MODKEY,                       XK_b,      togglebar,      {0} },
	{ KeyPress,    MODKEY,                       XK_j,      focusstack,     {.i = -1 } },
	{ KeyPress,    MODKEY,                       XK_k,      focusstack,     {.i = +1 } },
	{ KeyPress,    MODKEY|ShiftMask,             XK_j,      movestack,      {.i = -1 } },
	{ KeyPress,    MODKEY|ShiftMask,             XK_k,      movestack,      {.i = +1 } },
	{ KeyPress,    MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ KeyPress,    MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ KeyPress,    MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ KeyPress,    MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ KeyPress,    MODKEY,                       XK_Return, zoom,           {0} },
	{ KeyPress,    MODKEY,                       XK_Tab,    view,           {0} },
	{ KeyPress,    MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ KeyPress,    MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ KeyPress,    MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ KeyPress,    MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ KeyPress,    MODKEY,                       XK_space,  setlayout,      {0} },
	{ KeyPress,    MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ KeyPress,    MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ KeyPress,    MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ KeyPress,    MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ KeyPress,    MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ KeyPress,    MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ KeyPress,    MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ KeyPress,    MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ KeyPress,    MODKEY,                       XK_Left,   viewtoleft,     {0} },
	{ KeyPress,    MODKEY,                       XK_Right,  viewtoright,    {0} },
	{ KeyPress,    MODKEY|ShiftMask,             XK_Left,   tagtoleft,      {0} },
	{ KeyPress,    MODKEY|ShiftMask,             XK_Right,  tagtoright,     {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ KeyPress,    MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

void
movestack(const Arg *arg) {
	Client *c = NULL, *p = NULL, *pc = NULL, *i;

	if(arg->i > 0) {
		/* find the client after selmon->sel */
		for(c = selmon->sel->next; c && (!ISVISIBLE(c) || c->isfloating); c = c->next);
		if(!c)
			for(c = selmon->clients; c && (!ISVISIBLE(c) || c->isfloating); c = c->next);

	}
	else {
		/* find the client before selmon->sel */
		for(i = selmon->clients; i != selmon->sel; i = i->next)
			if(ISVISIBLE(i) && !i->isfloating)
				c = i;
		if(!c)
			for(; i; i = i->next)
				if(ISVISIBLE(i) && !i->isfloating)
					c = i;
	}
	/* find the client before selmon->sel and c */
	for(i = selmon->clients; i && (!p || !pc); i = i->next) {
		if(i->next == selmon->sel)
			p = i;
		if(i->next == c)
			pc = i;
	}

	/* swap c and selmon->sel selmon->clients in the selmon->clients list */
	if(c && c != selmon->sel) {
		Client *temp = selmon->sel->next==c?selmon->sel:selmon->sel->next;
		selmon->sel->next = c->next==selmon->sel?c:c->next;
		c->next = temp;

		if(p && p != c)
			p->next = c;
		if(pc && pc != selmon->sel)
			pc->next = selmon->sel;

		if(selmon->sel == selmon->clients)
			selmon->clients = c;
		else if(c == selmon->clients)
			selmon->clients = selmon->sel;

		arrange(selmon);
	}
}

