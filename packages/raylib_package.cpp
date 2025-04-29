#include "raylib_package.h"

#include "class.h"
#include "interpreter.h"
#include "lexer.h"
#include "log.h"
#include "package.h"
#include "parser.h"
#include "raylib.h"
#include "resolver.h"
#include "statement.h"
#include "token.h"
#include <memory>


namespace minik {

Ref<Object> find_field(const FieldsMap& fields, const std::string& name) {
	auto it = fields.find(name);
	if (it != fields.end()) {
		return it->second;
	}
	return nullptr;
}
Color get_color_from_arg(const Ref<Object>& argument) {
	Color color = Color{0, 0, 0, 255};
	if (argument->is_instance()) {
		Ref<MinikInstance> cal = argument->as_instance();
		NativeClass &nc = cal->nclas;
		color.r = find_field(nc.fields, "r")->as_double(); 
		color.g = find_field(nc.fields, "g")->as_double(); 
		color.b = find_field(nc.fields, "b")->as_double(); 
		color.a = find_field(nc.fields, "a")->as_double(); 
	}
	return color;
}


void define_and_register_functions(FieldsMap& m) {

DEFINE_AND_REGISTER(InitWindow, 3, {
	int width =  arguments[0]->as_double();
	int height = arguments[1]->as_double();
	std::string& title = arguments[2]->as_string();

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(width, height, title.c_str());
	SetTargetFPS(144);
	return nullptr;
})

DEFINE_AND_REGISTER(WindowShouldClose, 0, {
	return CreateRef<Object>((bool)WindowShouldClose());
})

DEFINE_AND_REGISTER(CloseWindow, 0, {
	CloseWindow();
	return nullptr;
})

DEFINE_AND_REGISTER(DrawText, 4, {
	int x = arguments[0]->as_double();
	int y = arguments[1]->as_double();
	int s = arguments[2]->as_double();
	std::string text = arguments[3]->to_string();

	DrawText(text.c_str(), x, y, s, WHITE);

	return nullptr;
})


DEFINE_AND_REGISTER(DrawCircle, 4, {
	int x = arguments[0]->as_double();
	int y = arguments[1]->as_double();
	int radius = arguments[2]->as_double();
	Color c = get_color_from_arg(arguments[3]);
	DrawCircle(x, y, radius, c);
	return nullptr;
})


DEFINE_AND_REGISTER(ClearBackground, 1, {
	Color color = get_color_from_arg(arguments[0]);
	ClearBackground(color);
	return nullptr;
})

DEFINE_AND_REGISTER(BeginDrawing, 0, {
	BeginDrawing();
	return nullptr;
})

DEFINE_AND_REGISTER(EndDrawing, 0, {
	EndDrawing();
	return nullptr;
})

DEFINE_AND_REGISTER(DrawRectangle, 5, {
	int x = arguments[0]->as_double();
	int y = arguments[1]->as_double();
	int width = arguments[2]->as_double();
	int height = arguments[3]->as_double();
	Color col = get_color_from_arg(arguments[4]);
	DrawRectangle(x, y, width, height, col);
	return nullptr;
});

DEFINE_AND_REGISTER(GetScreenHeight, 0, {
    return CreateRef<Object>((double)GetScreenHeight());
});

DEFINE_AND_REGISTER(GetScreenWidth, 0, {
    return CreateRef<Object>((double)GetScreenWidth());
});

DEFINE_AND_REGISTER(IsKeyDown, 1, {
	int key = arguments[0]->as_double();
    return CreateRef<Object>(IsKeyDown(key));
})

}



Ref<NativeClass> CreateVector2(double x, double y) {
	Ref<NativeClass> vector2 = CreateRef<NativeClass>("Vector2");
	vector2->add_field("x", CreateRef<Object>((double)x));
	vector2->add_field("y", CreateRef<Object>((double)y));
	return vector2;
}

class mpcRVec2 : public MinikCallable {
public:
	virtual int arity() override { return -1; }
	virtual std::string to_string() const override { return "<fn native Vector2 constructor>"; }
	virtual Ref<Object> call(Interpreter& interpreter, const Arguments& arguments) override {
		double x = 0;
		double y = 0;
		if (arguments.size() == 2) {
			x = arguments[0]->as_double();
			y = arguments[1]->as_double();
		}
		return CreateRef<Object>(CreateVector2(x, y)->call(interpreter, arguments));
	}
};


Ref<NativeClass> CreateColor(double r, double g, double b, double a) {
	Ref<NativeClass> col = CreateRef<NativeClass>("Color");
	col->add_field("r", CreateRef<Object>(r));
	col->add_field("g", CreateRef<Object>(g));
	col->add_field("b", CreateRef<Object>(b));
	col->add_field("a", CreateRef<Object>(a));
	return col;
}

class mcRCol : public MinikCallable {
public:
	virtual int arity() override { return -1; }
	virtual std::string to_string() const override { return "<fn native Color constructor>"; }
	virtual Ref<Object> call(Interpreter& interpreter, const Arguments& arguments) override {
		double c[4] = {0,0,0,255};
		for (int i = 0; i < arguments.size(); ++i) {
			c[i] = arguments[i]->as_double();
		}
		return CreateRef<Object>(CreateColor(c[0],c[1],c[2],c[3])->call(interpreter, arguments));
	}
};


void define_KEYs(FieldsMap& f) {
	f["KEY_W"] = CreateRef<Object>((double)KEY_W);
	f["KEY_A"] = CreateRef<Object>((double)KEY_A);
	f["KEY_S"] = CreateRef<Object>((double)KEY_S);
	f["KEY_D"] = CreateRef<Object>((double)KEY_D);
	f["KEY_UP"]    = CreateRef<Object>((double)KEY_UP);
	f["KEY_LEFT"]  = CreateRef<Object>((double)KEY_LEFT);
	f["KEY_DOWN"]  = CreateRef<Object>((double)KEY_DOWN);
	f["KEY_RIGHT"] = CreateRef<Object>((double)KEY_RIGHT);
	f["KEY_CAPS_LOCK"] = CreateRef<Object>((double)KEY_CAPS_LOCK);
}


void RaylibPackage::ImportPackage(const Ref<Environment>& environment, const std::string& as) {
	std::string package_import_as = name;
	if (!as.empty()) {
		package_import_as = as;
	}

	Ref<MinikNamespace> ns = CreateRef<MinikNamespace>(package_import_as, nullptr);
	define_and_register_functions(ns->fields);

	Ref<NativeClass> vector2 = CreateVector2(0, 0);
	vector2->add_field("Vector2", CreateRef<Object>(CreateRef<mpcRVec2>()));

	// NOTE: this is under ns, can be accessed like: 
	// v := Raylib.Vector2(1,2);
	ns->fields["Vector2"] = CreateRef<Object>(vector2);

	// NOTE: this is global, can be accessed like: 
	// v := Vector2(1,2);
	// environment->predefine(Token{CLASS,"Vector2",nullptr,0}, CreateRef<Object>(vector2));


	Ref<NativeClass> col = CreateColor(1,1,1,1);
	col->add_field("Color", CreateRef<Object>(CreateRef<mcRCol>()));
	ns->fields["Color"] = CreateRef<Object>(col);

	define_KEYs(ns->fields);

	environment->predefine(Token(IDENTIFIER, ns->name, {}, 0), CreateRef<Object>(ns));
}


}

