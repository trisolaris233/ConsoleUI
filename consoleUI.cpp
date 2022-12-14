// consoleUI.cpp : Defines the entry point for the console application.
//

#include <locale>
#include <memory>
#include <vector>
#include <iostream>
#include <utility>

#include <conio.h>

#include <Object.h>
#include <General.h>
#include <Frame.h>
#include <Tag.h>
#include <Button.h>
#include <Signal.h>
#include <TextBox.h>
#include <Controller.h>
#include <codecvt>  

// linux下为"zh_CN.GBK"  
#define GBK_NAME ".936"  

using namespace sweet::cmd;

void onSelect(MoveController* move, Object* obj) {
	if (obj) {
		obj->setSelected(false);
	}
	if (move && move->current()) {
		move->current()->setSelected(true);
	}

}

void KeyReception(Controller* sender, int code, bool state) {
	MoveController* move = dynamic_cast<MoveController*>(sender->getFriendWithTag("move"));
	if (!move)
		return;
	if (state) {
		switch (code) {
		case VK_UP:
			move->goUp();
			break;
		case VK_DOWN:
			move->goDown();
			break;
		case VK_LEFT:
			move->goLeft();
			break;
		case VK_RIGHT:
			move->goRight();
			break;
		}
	}
}

void setSelected(Object* obj, bool state) {
	//if (state) {
	Frame* pfmr = dynamic_cast<Frame*>(obj);
	Tag* ptag = dynamic_cast<Tag*>(obj);
	if (ptag) {
		if (!state) {
			ptag->setForeColor(makeLpStream(cast2UChr(WHITE)));
		}
		else {
			ptag->setForeColor(makeLpStream(cast2UChr(RED)));
		}
	}
	if (pfmr) {
		if (state) 
			pfmr->setPen(makeRdStream(cast2UChr(BLACK), cast2UChr(WHITE)));
		else
			pfmr->setPen(makeLpStream(cast2UChr(WHITE)));
	}
	obj->refresh();
	//}
}

void testMove() {
	using namespace sweet;
	Frame fmr(
		nullptr,
		Rect(0, 0, toShort(tool::GetWindowWidth()), toShort(tool::GetWindowHeight())),
		0,
		Bucket(makeLpStream(L'*'))
	);
	Button btn(
		&fmr,
		Rect(5,4,30,9),
		L"恶膜谋命命秒没", 
		makeLpStream(cast2UChr(FOREGROUND_INTENSITY)),
		makeLpStream(cast2UChr(BACKGROUND_BLUE)),
		1,
		makeLpStream(L'の')
	);
	Button btn2(
		&fmr,
		Rect(5, 16, 30, 21),
		L"恶膜谋命命秒没",
		makeLpStream(cast2UChr(FOREGROUND_INTENSITY)),
		makeLpStream(cast2UChr(BACKGROUND_BLUE)),
		1,
		makeLpStream(L'*')
	);
	Button btn3(
		&fmr,
		Rect(28, 12, 58, 17),
		L"恶膜谋命命秒没",
		makeLpStream(cast2UChr(FOREGROUND_INTENSITY)),
		makeLpStream(cast2UChr(BACKGROUND_BLUE)),
		1,
		makeLpStream(L'-')
	);
	Button btn4(
		&fmr,
		Rect(32, 17, 62, 22),
		L"恶膜谋命命秒没",
		makeLpStream(cast2UChr(FOREGROUND_INTENSITY)),
		makeLpStream(cast2UChr(BACKGROUND_BLUE)),
		1,
		makeLpStream(L'烫')
	);

	KeyLastController keyLastController;
	auto slot2 = keyLastController.sig_KeyPressed.connect(KeyReception);
	auto slot3 = btn.sig_SetSelected.connect(setSelected);
	auto slot4 = btn2.sig_SetSelected.connect(setSelected);
	auto slot5 = btn3.sig_SetSelected.connect(setSelected);
	auto slot6 = btn4.sig_SetSelected.connect(setSelected);

	MoveController	  moveController(&fmr);
	auto slot1 = moveController.sig_onSelection.connect(onSelect);
	keyLastController.addFriend(&moveController, "move");

	fmr.show();
	btn.show();
	btn2.show();
	btn3.show();
	btn4.show();


	moveController.go();
	keyLastController.go();

	for (;;)Sleep(10);
	
}

void testDrawRect() {
	Bucket border(makeLpStream(L'*'));
	Bucket blank(makeLpStream(L'*'));
	sweet::LoopStream<char> fixer(' ');
	Pen	   borderPen(makeLpStream(sweet::cmd::cast2UChr(GREEN)));
	Pen    blankPen(makeLpStream(sweet::cmd::cast2UChr(RED)));
	sweet::cmd::tool::DrawRectAbs(
		Rect{ 1,1,10,10 },
		*border.chrs(),
		*blank.chrs(),
		*borderPen.color(),
		*blankPen.color(),
		toShort(2),
		fixer
	);
}

wchar_t* trans(const char * ch, int type = CP_ACP) {
	int len = MultiByteToWideChar(type, 0, ch, -1, nullptr, 0);
	wchar_t *str = new wchar_t[len + 1];
	wmemset(str, 0, len + 1);
	MultiByteToWideChar(type, 0, ch, -1, str, len);
	return str;
}

void testConsoleInput() {
	char ch;
	wchar_t wch;
	std::string  str;
	std::wstring wstr;
	while ((ch = _getch()) != 'e') {
		str += ch;
	}
}

int main2()
{
	//using namespace std;

	//using WCHAR_GBK = codecvt_byname<wchar_t, char, mbstate_t>;
	//using WCHAR_UTF8 = codecvt_utf8<wchar_t>;
	//// 定义一个utf8字符串  
	//string result = "中国人";
	//// gbk与unicode之间的转换器  
	//wstring_convert<WCHAR_GBK>  cvtGBK(new WCHAR_GBK(GBK_NAME));
	//// utf8与unicode之间的转换器  
	//wstring_convert<WCHAR_UTF8> cvtUTF8;
	//// 从utf8转换为unicode  
	//wstring ustr = cvtGBK.from_bytes(result);
	//wcout.imbue(locale(""));
	//wcout << ustr;
	//getchar();
	//return 0;
}

void testTextBox_KeyReception(KeyLastController* sender, int code, bool state) {
	MoveController* move = dynamic_cast<MoveController*>(sender->getFriendWithTag("move"));
	if (!move)
		return;
	// 正常的上下左右功能
	if (state) {
		switch (code) {
		case VK_UP:
			move->goUp();
			break;
		case VK_DOWN:
			move->goDown();
			break;
		case VK_LEFT:
			move->goLeft();
			break;
		case VK_RIGHT:
			move->goRight();
			break;

		case VK_RETURN:
			//std::cout << "enter textbox editing mode\n";
			//std::cout << move->current()->identifier() << std::endl;
			if (4 == move->current()->identifier()) {	// 如果当前选择的是一个Textbox
				TextBox* tbx = dynamic_cast<TextBox*>(move->current());
				if (tbx) {
					sender->pause();	// 暂停keyController
					tbx->startInput();	// 开始监听按钮
				}
				break;
			}
		}
	}
}
void testTextBox_onSelection(MoveController* move, Object* old) {
	if (old)
		old->setSelected(false);
	if (move && move->current())
		move->current()->setSelected(true);
}
void testTextBox_TextBoxsetSelected(Object* obj, bool state) {
	if (state) {
		dynamic_cast<TextBox*>(obj)->setPen(makeRdStream(cast2UChr(sweet::cmd::BLACK), cast2UChr(sweet::cmd::WHITE)));
	}
	else {
		dynamic_cast<TextBox*>(obj)->setPen(makeLpStream(cast2UChr(sweet::cmd::WHITE)));
	}
	obj->refresh();
}
void testTextBox_TagSetSelected(Object* obj, bool state) {
	if (state) {
		dynamic_cast<Tag*>(obj)->setForeColor(makeLpStream(cast2UChr(sweet::cmd::RED)));
	}
	else {
		dynamic_cast<Tag*>(obj)->setForeColor(makeLpStream(cast2UChr(sweet::cmd::WHITE)));
	}
	obj->refresh();
}

void testTextBox() {
	using namespace sweet;
	tool::SetCursorVisible(false);	// 设置光标不可见
	Frame fmr(
		nullptr,
		Rect(0, 0, toShort(tool::GetWindowWidth()), toShort(tool::GetWindowHeight())),
		0,
		Bucket(makeLpStream(L'*'))
	);
	TextBox tbx(&fmr, Rect(5, 5, 60, 25));
	Tag		tag(&fmr, Rect(toShort(tool::GetWindowWidth())/2, 3, 0, 0), L"某TextBox测试");
	tag.setAutoStrech(true);	// 设置Tag自动拉伸

	MoveController move(&fmr);	// 注册moveController
	KeyLastController keyWatcher;	// 注册keyLastController

	// 互相添加通讯录
	move.addFriend(&keyWatcher, "key");
	keyWatcher.addFriend(&move, "move");

	// 监听结束之信号槽
	auto testTextBox_TypeFinished = [&](TextBox* tbox) {
		keyWatcher.go();
	};

	// 链接信号槽
	auto slot1 = tbx.sig_TypeFinished.connect(testTextBox_TypeFinished);
	auto slot2 = keyWatcher.sig_KeyPressed.connect(testTextBox_KeyReception);
	//auto slot3 = tbx.sig_SetSelected.connect(setSelected);
	//auto slot4 = tag.sig_SetSelected.connect(setSelected);
	auto slot5 = move.sig_onSelection.connect(testTextBox_onSelection);
	auto slot6 = tbx.sig_SetSelected.connect(testTextBox_TextBoxsetSelected);
	auto slot7 = tag.sig_SetSelected.connect(testTextBox_TagSetSelected);
	
	// 启动controller
	move.go();
	keyWatcher.go();

	// 显示
	fmr.show();
	tag.show();
	tbx.show();

	for (;;)Sleep(100);
}

void testBindMember() {
	class B
	{
	signals:
		sweet::Signal<void(B*)> sig_Test;

	public:
		void throwSignal() { sig_Test(this); }
	};
	class A
	{
	public:
		B objB;

	public:
		void FuncD() {
			objB.sig_Test.connect(this, &A::FuncB);
		}
		void FuncC() {
			objB.throwSignal();
		}
		void FuncB(B*pb) {
			printf("66666666\n");
		}
		void FuncA(int v1, float v2, std::string str)
		{
			printf("%d %f %s\n", v1, v2, str.c_str());
		}
	};

	A objA;
	objA.FuncD();

	//objA.objB.sig_Test.connect(&objA, &A::FuncB);
	objA.objB.throwSignal();
	objA.FuncC();
}

int main()
{
	using namespace std;
	testTextBox();
	std::cin.get();
    return 0;
}

