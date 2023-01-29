/*!
 * @file
 * @brief File which includes different phrases for tex dump.
 */

#ifdef TEX_PHRASES_H_
#error phrases.h can be included only one time.
#endif

#define TEX_PHRASES_H_

#include "tex.h"


static const char* const TEX_PREAMBLE =
"\\documentclass[a4paper,oneside,final,12pt,russian]{extarticle}\n"
"\\usepackage[utf8]{inputenc}\n"
"\\usepackage[T2A,T1]{fontenc}\n"
"\\usepackage[russian]{babel}\n"
"\\usepackage{vmargin}\n"
"\\usepackage{indentfirst}\n"
"\\usepackage{amsmath}\n"
"\\usepackage{amsfonts}\n"
"\\usepackage{hyperref}\n"
"\\usepackage{cmap}\n"
"\\usepackage{listings}\n"
"\\usepackage{ulem}\n"
"\\usepackage{breqn}\n"
"\\setpapersize{A4}\n"
"\\setmarginsrb{2cm}{1.5cm}{1cm}{1.5cm}{0pt}{0mm}{0pt}{13mm}\n"
"\n"
"\\usepackage{blindtext}\n"
"\\usepackage{setspace}\n"
"\\sloppy\n"
"\\hyphenpenalty=5000\n"
"\n"
"\\usepackage{titling}\n"
"\\setlength{\\droptitle}{-1cm}\n"
"\n"
"\\title{\\normalsize \\bf РЕШЕНИЕ СЛОЖНЫХ ЗАДАЧ \\\\ НАХОЖДЕНИЕ ПРОИЗВОДНОЙ И ФОРМУЛЫ ТЕЙЛОРА}\n"
"\\author{\\small Иванов Денис \\\\ Б05--132}\n"
"\n"
"\\newcommand{\\e}{\\mathsf{e}}"
"\n"
"\\begin{document}\n"
"\n"
"\\maketitle\n"
"\n"
"Для разложения выражения $";

static const char* const TEX_PREAMBLE_1 =
"$ в ряд Тейлора нам нужно найти $";

static const char* const tEX_PREAMBLE_2 =
"$ первых производных. Ну что ж, давайте сделаем это!\n"
"По крайней мере попробуем)\n"
"\n";


static const char* const TEX_TAYLOR =
"\\section{}\n"
"Таким образом, разложив функцию по Тейлору в точке $";


static const char* const TEX_TAYLOR_1 =
"$, получим следующее:\n";


static const char* const TEX_FINAL =
"Вот такая вот унылая фигня у нас получилась.\n"
"\n"
"Спасибо за внимание))\n"
"\\end{document}\n";


static const char* const TEX_PHRASES[TEX_CONTEXTS_AMOUNT][5] =
{
	[CONTEXT_BEGIN_DIFF] =
	{
		"\\section{}\nДавайте возьмём производную $",
		"\\section{}\nСпросим у шаманов, чему равна производная \n"
		"этого выражения $",
		NULL
	},
	[CONTEXT_BEGIN_DIFF_1] =
	{
		"$.\n\n",
		"$. Во имя котиков мы обязаны сделать это!\n\n",
		NULL
	},
	[CONTEXT_END_DIFF] =
	{
		"Ух... Наконец-то мы сделали это!\n\n",
		"Это было очень сложно. Не только лишь все способны \n"
		"на это.\n\n",
		"Здесь читатель может сделать небольшой перерыв \n"
		"и пойти попить айрана.\n\n",
		NULL
	},
	[CONTEXT_DIFF_NUMBER] =
	{
		"Тривиальный переход ~--- это всё, на что мы способны",
		"Производная любого числа равна числу, не существующему в природе.",
		"Чему равна производная числа??? НОЛЬ, НОЛЬ, НОЛЬ!!!",
		NULL
	},
	[CONTEXT_DIFF_VAR] =
	{
		"Нарисуем кишечную палочку",
		"Притворимся, что знаем матан))",
		NULL
	},
	[CONTEXT_DIFF_FUNC] =
	{
		"Чему равна производная сложной функции?.. А хрен его знает.",
		"В СССР дети знали это с детского сада!",
		"Раз, два, три, четыре, пять.\\\\ \n"
		"Вышли скобки погулять.\\\\ \n"
		"И поняли, вернувшись домой.\\\\ \n"
		"Производную от аргумента взял кто-то другой.",
		NULL
	},
	[CONTEXT_DIFF_OP] =
	{
		"Согласно Википедии",
		"Легко догадаться, что",
		"А что если Господь создал этот мир только для того, \n"
		"чтобы мы посчитали за него следующую производную:",
		"Ах если бы сложение стало делением, \\\\ \n"
		"А, может, даже умножением! \\\\ \n"
		"Вот было бы прекрасно,\\\\ \n"
		"Если бы муссонные ветра дули не напрасно. \\\\ \n"
		"Тогда может быть и был бы смысл в этой жизни,\\\\ \n"
		"И равен бы он был морали этих строк, которой нет...\\\\ \n"
		"Так о чём же это я? Да ни о чём, собственно говоря.\n"
		"Я просто хочу удалить этот файл с бессмыденным матаном \n"
		"и никогда его больше не видеть. :(",
		NULL
	},
	[CONTEXT_OPTIMIZE] =
	{
		"\n\nСамое время привести наше выражение к виду, \n"
		"удобному для хомячков, то есть нас с вами.\n",
		"\n\nНемоного оптимизируем: \n",
		"\n\nБлагодаря интуиции поймём, что\n",
		NULL
	},
};
