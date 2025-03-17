#include "raylib_package.h"

#include "class.h"
#include "interpreter.h"
#include "lexer.h"
#include "log.h"
#include "parser.h"
#include "raylib.h"
#include "resolver.h"
#include "statement.h"
#include "token.h"


namespace minik {

#define DEFINE_FUNCTION(name, arit, body)  \
class mc##name : public MinikCallable { public:  \
	virtual int arity() override { return arit; }\
	virtual std::string to_string() const override { return "<fn native " #name ">"; } \
	virtual Ref<Object> call(Interpreter& interpreter, const Arguments& arguments) override { body } };


DEFINE_FUNCTION(InitWindow, 3, {
	int width =  arguments[0]->as_double();
	int height = arguments[1]->as_double();
	std::string& title = arguments[2]->as_string();

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(width, height, title.c_str());
	SetTargetFPS(144);
	return nullptr;
})

DEFINE_FUNCTION(WindowShouldClose, 0, {
	return CreateRef<Object>((bool)WindowShouldClose());
})

DEFINE_FUNCTION(CloseWindow, 0, {
	CloseWindow();
	return nullptr;
})

DEFINE_FUNCTION(DrawText, 3, {
	int x = arguments[0]->as_double();
	int y = arguments[1]->as_double();
	std::string text = arguments[2]->to_string();

	DrawText(text.c_str(), x, y, 20, WHITE);

	return nullptr;
})


DEFINE_FUNCTION(DrawCircle, 3, {
	int x = arguments[0]->as_double();
	int y = arguments[1]->as_double();
	int radius = arguments[2]->as_double();


	Color c = BLUE;
	if (true) {
		c = (CLITERAL(Color){ 0, 228, 228, 255 });
	}
	DrawCircle(x, y, radius, c);

	return nullptr;
})


DEFINE_FUNCTION(ClearBackground, 1, {
	std::string color_str = arguments[0]->to_string();
	Color color = (color_str == "BLACK") ? BLACK : WHITE;

	ClearBackground(color);

	return nullptr;
})

DEFINE_FUNCTION(BeginDrawing, 0, {
	BeginDrawing();
	return nullptr;
})

DEFINE_FUNCTION(EndDrawing, 0, {
	EndDrawing();
	return nullptr;
})

DEFINE_FUNCTION(DrawRectangle, 4, {
	int x = arguments[0]->as_double();
	int y = arguments[1]->as_double();
	int width = arguments[2]->as_double();
	int height = arguments[3]->as_double();
	DrawRectangle(x, y, width, height, WHITE);
	return nullptr;
});

DEFINE_FUNCTION(GetScreenHeight, 0, {
    return CreateRef<Object>((double)GetScreenHeight());
});

DEFINE_FUNCTION(GetScreenWidth, 0, {
    return CreateRef<Object>((double)GetScreenWidth());
});



Ref<NativeClass> CreateVector2(double x, double y) {
	Ref<NativeClass> vector2 = CreateRef<NativeClass>("Vector2");
	vector2->add_field("x", CreateRef<Object>(x));
	vector2->add_field("y", CreateRef<Object>(y));
	return vector2;
}

class mcRVec2 : public MinikCallable {
public:
	virtual int arity() override { return -1; }
	virtual std::string to_string() const override { return "<fn native Vector2 constructor>"; }
	virtual Ref<Object> call(Interpreter& interpreter, const Arguments& arguments) override {
		int x = 0;
		int y = 0;
		if (arguments.size() == 2) {
			x = arguments[0]->as_double();
			y = arguments[1]->as_double();
		}
		return CreateRef<Object>(CreateVector2(x, y)->call(interpreter, arguments));
	}
};


void RaylibPackage::ImportPackage(const Ref<Environment>& environment, const std::string& as) {
	std::string package_import_as = name;
	if (!as.empty()) {
		package_import_as = as;
	}

	Ref<MinikNamespace> ns = CreateRef<MinikNamespace>(package_import_as, nullptr);

	ns->fields["InitWindow"]        = CreateRef<Object>(CreateRef<mcInitWindow>());
	ns->fields["WindowShouldClose"] = CreateRef<Object>(CreateRef<mcWindowShouldClose>());
	ns->fields["CloseWindow"]       = CreateRef<Object>(CreateRef<mcCloseWindow>());
	ns->fields["DrawText"]          = CreateRef<Object>(CreateRef<mcDrawText>());
	ns->fields["DrawCircle"]        = CreateRef<Object>(CreateRef<mcDrawCircle>());
	ns->fields["DrawRectangle"]     = CreateRef<Object>(CreateRef<mcDrawRectangle>());
	ns->fields["ClearBackground"]   = CreateRef<Object>(CreateRef<mcClearBackground>());
	ns->fields["BeginDrawing"]      = CreateRef<Object>(CreateRef<mcBeginDrawing>());
	ns->fields["EndDrawing"]        = CreateRef<Object>(CreateRef<mcEndDrawing>());
	ns->fields["GetScreenHeight"]   = CreateRef<Object>(CreateRef<mcGetScreenHeight>());
	ns->fields["GetScreenWidth"]    = CreateRef<Object>(CreateRef<mcGetScreenWidth>());


	Ref<NativeClass> vector2 = CreateVector2(0, 0);
	vector2->add_field("Vector2", CreateRef<Object>(CreateRef<mcRVec2>()));

	// NOTE: this is under ns, can be accessed like: 
	// v := Raylib.Vector2(1,2);
	ns->fields["Vector2"] = CreateRef<Object>(vector2);

	// NOTE: this is global, can be accessed like: 
	// v := Vector2(1,2);
	// environment->predefine(Token{CLASS,"Vector2",nullptr,0}, CreateRef<Object>(vector2));

	environment->predefine(Token(IDENTIFIER, ns->name, {}, 0), CreateRef<Object>(ns));
}


}

