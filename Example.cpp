#include "Terminal.hpp"

using namespace terminal;

int main()
{
	Console con;

	for (TextColor t = TextColor::Start; t <= TextColor::End; t = (TextColor)((int)t + 1))
	{
		for (BackgroundColor b = BackgroundColor::Start; b <= BackgroundColor::End; b = (BackgroundColor)((int)b + 1))
		{
			con << Style("[Style]").color(t).color(b).width(10) << " ";
		}
		con << "\n";
	}

	con << Style("------------------------\n");
	con << Style("A").width(20).left() << "\n";
	con << Style("A").width(20).center() << "\n";
	con << Style("A").width(20).right() << "\n";

	for (TextColor t = TextColor::Start; t <= TextColor::End; t = (TextColor)((int)t + 1))
	{
		for (BackgroundColor b = BackgroundColor::Start; b <= BackgroundColor::End; b = (BackgroundColor)((int)b + 1))
		{
			con << Style(" [Style] ").color(t).color(b).bold() << " ";
		}
		con << "\n";
	}

	con.notice("[Notice] Hey this is notice");
	con.warning("[Warning] Hey this is warning");
	con.error("[Error] Hey this is error");
	con.success("[OK] Hey this is success");

	Style one;
	Style two;

	one.bgBlue().white().bold();
	two.bgWhite().red().width(30).center();

	con << one.text("Style one")
		<< two.text("Style two")
		<< one.text("Again style one");

	return 0;
}