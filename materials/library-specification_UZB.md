# BrickGame o’yinlar to’plamidan o’yin kutubxonasi uchun spetsifikatsiya

Bu vazifa BrickGame seriyasidagi birinchi vazifadir. Hammasi bo’lib to’rtta loyiha bo’ladi, ularning har biri o’z o’yini va o’z texnologiyalariga ega. Ammo yangi loyihalarni ishlab chiqishdan tashqari, eski o’yinlarni qo’llab-quvvatlash va eski loyihalarga yangi o’yinlarni qo’llab-quvvatlash kerak bo’ladi. Bu safar interfeys konsol bo’ladi, keyingi safar desktop bo’ladi va hokazo. Eski va yangi o’yinlarni qo’llab-quvvatlash uchun kelajakda topshiriligan loyihalarni qayta yozishga to’g’ri kelmasligi uchun API interfeyslari va kutubxonalari qanday tuzilganligini oldindan hal qilish kerak.

O’yin maydoni o’nga yigirma hajmidagi matritsa sifatida ifodalanadi. Matritsaning har bir elementi o’yin maydonining “pikseliga” to’g’ri keladi va ikkita holatdan birida bo’lishi mumkin: bo’sh va to’ldirilgan. O’yin maydoniga qo’shimcha ravishda, har bir o’yinda qo’shimcha ma’lumotlar mavjud bo’lib, ular o’yin maydonining o’ng tomonidagi yon panelda ko’rsatiladi. O’yin davomida ishlatilmaydigan qo’shimcha ma’lumotlar uchun joy egalari ko’rsatilishi kerak.

Har bir o’yin kutubxonasida foydalanuvchi ma’lumotlarini qabul qiladigan funksiya bo’lishi kerak. Konsolda sakkizta jismoniy tugma mavjud: o’yinni boshlash, to’xtatib turish, o’yinni tugatish, harakat va to’rtta strelka.

`userInput` funksiyasi kirish sifatida foydalanuvchi `action` harakatini va klavishni bosib ushlab turish uchun javob beradigan qo’shimcha `hold` parametrni oladi.

`updateCurrentState` funksiyasi interfeysda ko’rsatish uchun ma’lumotlarni olish uchun mo’ljallangan. U o’yinning joriy holati haqida ma’lumotni o’z ichiga olgan tuzilmani qaytaradi. Masalan, tetris uchun taymerning tugashi shaklning bir qator pastga siljishiga olib keladi. Interfeysni yangilab turish uchun ushbu funktsiyani interfeysdan ma’lum vaqt oralig’ida chaqirish kerak.

```c
typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

void userInput(UserAction_t action, bool hold);

GameInfo_t updateCurrentState();
```

E’tibor bering, `GameInfo_t` o’yinining joriy holati haqidagi ma’lumot o’yin kutubxonasi ichida statistik obyekt bilan ifodalanishi mumkin.