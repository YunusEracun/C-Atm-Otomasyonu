#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

typedef struct Musteri {
     int musteriNo;
     char tc[20];
     char adSoyad[30];
     char adres[30];
     char telefon[20];
     char eposta[30];
     int sifre ;
     float bakiye;
     float tlBakiye;
     float usdBakiye;
     float euroBakiye;
     int sifreDurum; // tek kullanimlik sifre yenilenirse =1 olacak
     int HesapDurum; // hesap dondurulursa =0 aktif olursa =1 olacak
     double limit;
} musteri;

typedef struct Hareket {
     int musteriNo;
     char adSoyad[30];
     char sube[20];
     char islem[60];
     time_t tarih;
     double hareketTutar;
} hareket ;

typedef struct {
    char musteriTc[20];
    double limit;
    double kullanilabilirLimit;
    double bakiye;
    char kartNo[20];
    char sonKullanma[10];
    int guvenlikKodu;
    char sifre[6];
} krediKarti;

void musteriEkle(){
    srand(time(0));	    // Rastgele sayi uretebilmek icin zaman bazli bir deger olusturma
    clearScreen();
    setColor(14);
    printFrame("                          MUSTERI EKLEME EKRANI                          "); printf("\n\n");
    resetColor();
	// Yeni bir musteri structi olusturulur.
	//Bu struct müsteri bilgilerini saklamak için kullanilir
	musteri m1;
	// Kullanicidan yeni müsteri bilgileri alinir
    setColor(10); printCentered("TC       : "); resetColor();
    setColor(12); scanf("%s",m1.tc); resetColor();
    setColor(10); printCentered("Ad Soyad : ");  resetColor();
    setColor(12); scanf(" %[^\n]",m1.adSoyad); resetColor();
    setColor(10); printCentered("Adres    : ");  resetColor();
    setColor(12); scanf(" %[^\n]", m1.adres); resetColor();
    setColor(10); printCentered("Telefon  : ");  resetColor();
    setColor(12); scanf("%s",m1.telefon); resetColor();
    setColor(10); printCentered("E-Posta  : ");  resetColor();
    setColor(12); scanf("%s",m1.eposta); resetColor();

	// Rastgele musteri numarasi ve sifre olusturma
    m1.musteriNo= rand() % 1000 + 1000;
    m1.sifre=rand() % 10000 + 10000;

    // Bakiye ve diger bilgileri baslangic degerleriyle ayarlama
    m1.tlBakiye = 0.0;
    m1.usdBakiye = 0.0;
    m1.euroBakiye = 0.0;
    m1.sifreDurum= 0;
    m1.HesapDurum= 1;

	// Musteri bilgilerini dosyaya ekleme
    FILE * ptr= fopen("musteriler.dat","a+b");
    fwrite( &m1, sizeof(musteri), 1, ptr);
    fclose(ptr);

    // Kullaniciya basariyla kaydedildigine dair bilgileri gosterme
    setColor(10);
    printCentered("\n\n"); printCentered("Musteri kaydi tamamlandi"); sleep(1);  printf("\n\n");
    printCentered("Tek kullanimlik sifre SMS olarak gonderildi"); sleep(1);  printf("\n\n"); resetColor();
    setColor(12); printCentered("gizli sifre ( %d )",m1.sifre); printf("\n\n"); resetColor();
    sleep(7); clearScreen();
}

void musteriSil(){
    srand(time(0));	  // Rastgele sayi uretebilmek için zaman bazli bir deger olusturma

    clearScreen();
    setColor(14);
    printFrame("             MUSTERI SILME EKRANI             "); printf("\n\n");
    resetColor();
    musteri m1;
    char tc[20];

    setColor(10); printCentered("TC : "); resetColor();
    setColor(12); scanf(" %[^\n]s", tc);  resetColor(); printf("\n\n");

    FILE * ptr= fopen("musteriler.dat","r+b");	// Musteri bilgilerini içeren dosyayi acma

    int sonuc=0;


	// Dosyadan musteri bilgilerini okuma ve kontrol
    while (fread ( &m1, sizeof(musteri), 1, ptr) !=NULL )
    {
        if(strcmp(tc,m1.tc)==0 )
        {
            sonuc=1;
            break;
        }
    }
    fclose(ptr);
    if(sonuc ==0 )
    {
    	// Eger musteri bulunamazsa hata mesaji yazdirma
        setColor(12); printCentered("%s numarali musteri kaydi bulunamadi !",tc); printf("\n"); resetColor();
    }
    else
    {
        char tercih;
        // Kullanicinin silmeyi onaylayip onaylamadigini sorma
        setColor(10); printCentered("%s kisisini silmek istediginize emin misiniz ?(E) : ",m1.adSoyad); resetColor();
        setColor(12); scanf(" %c",&tercih); resetColor(); printf("\n\n");
        getchar();
        if(tercih == 'e' || tercih == 'E' )
        {
        // Musteri bilgilerini içeren dosyayi ve yedek dosyayi acma
        ptr= fopen("musteriler.dat","r+b");
        FILE *yptr=fopen("yedek.dat","w+b");

        // Silinen musteri disinda diger musteri bilgilerini yedek dosyaya yazma
        int silSonuc=0;

        while (fread ( &m1, sizeof(musteri), 1, ptr) !=NULL )
     {
        if(strcmp(tc,m1.tc) ==0 )
        {
            silSonuc=1;
        }
        else
        fwrite ( &m1, sizeof(musteri), 1, yptr);
      }
      fclose(ptr);
      fclose(yptr);

      if(silSonuc==0 ){
        setColor(12);  printCentered("Silme islemi basarisiz !"); printf("\n"); resetColor();
        sleep(3); clearScreen();
      }
      else
      {
    	// Dosyayi silme ve yedegi orjinal dosya adiyla yeniden adlandirma
          remove("musteriler.dat");
          rename("yedek.dat","musteriler.dat");
          setColor(10);   printCentered("%s numarali kisi silindi",tc);  resetColor();
          sleep(3); clearScreen();
      }
        }
        else
        {
            setColor(12);   printCentered("Silme islemi iptal edilmistir"); printf("\n"); resetColor();
            sleep(3); clearScreen();
        }
    }
}

void hesapDondur(){
    srand(time(0));	// Rastgele sayi uretebilmek icin zaman bazli bir deger olusturma

    clearScreen();
    setColor(14);
    printFrame("             MUSTERI HESAP DONDURMA EKRANI             "); printf("\n\n");
    resetColor();
    musteri m1;
    char tc[20];
    setColor(10); printCentered("TC : "); resetColor();
	setColor(12); scanf(" %[^\n]s", tc); resetColor();

    FILE *ptr = fopen("musteriler.dat", "r+b");	// Musteri bilgilerini iceren dosyayi acma
    int sonuc = 0, sayac;

	// Dosyadan musteri bilgilerini okuma ve kontrol
    for (sayac = 0; fread(&m1, sizeof(musteri), 1, ptr) != NULL; sayac++) {
        if (strcmp(tc, m1.tc) == 0) {
            sonuc = 1;
            break;
        }
    }
    if (sonuc == 0) {
        setColor(12); printCentered("%s numarali musteri kaydi bulunamadi !", tc); sleep(5); printf("\n\n"); resetColor();
    } else {
    	// Dosyayi basa alip, bulunan musterinin hesap durumunu degistirme ve dosyaya yazma
        rewind(ptr);
        fseek(ptr, sayac * sizeof(musteri), SEEK_SET);
        m1.HesapDurum = 0;	// Hesap donduruldugunda HesapDurumunu 0 olarak güncelleme
        fwrite(&m1, sizeof(musteri), 1, ptr);
        setColor(10); printf("\n\n"); printCentered("Islem Basariyla Gerceklestirildi."); sleep(1); printf("\n");
        printCentered("Hesap donduruldu.."); sleep(5); printf("\n\n"); resetColor(); clearScreen();
    }
    fclose(ptr); // Dosyayi kapatma
}

void hesapAktiflestir(){
    srand(time(0));	 // Rastgele sayi uretebilmek icin zaman bazli bir deger olusturma

    clearScreen();
    setColor(14);
    printFrame("             MUSTERI HESAP AKTIFLESTIRME ISLEMLERI             "); printf("\n\n");
    resetColor();
    musteri m1;
    char tc[20];
    setColor(10); printCentered("TC : "); resetColor();
	setColor(12); scanf(" %[^\n]s", tc); resetColor(); printf("\n\n");

    FILE *ptr = fopen("musteriler.dat", "r+b");	// Musteri bilgilerini içeren dosyayi acma
    int sonuc = 0, sayac;	// Musteri bulma icin kullanilacak degiskenler

	// Dosyadan musteri bilgilerini okuma ve kontrol
    for (sayac = 0; fread(&m1, sizeof(musteri), 1, ptr) != NULL; sayac++) {
        if (strcmp(tc, m1.tc) == 0) {
            sonuc = 1;
            break;
        }
    }
    if (sonuc == 0) {
        setColor(12); printCentered("%s numarali musteri kaydi bulunamadi !", tc); printf("\n\n"); resetColor();
    } else {
    	// Dosyayi basa alip, bulunan musterinin hesap durumunu degistirme ve dosyaya yazma
        rewind(ptr);
        fseek(ptr, sayac * sizeof(musteri), SEEK_SET);
        m1.HesapDurum = 1;	// Hesap aktiflestirildiginde HesapDurumunu 1 olarak guncelleme
        fwrite(&m1, sizeof(musteri), 1, ptr);
        setColor(10); printCentered("Islem Basariyla Gerceklestirildi."); sleep(1); printf("\n");
        printCentered("Hesap aktiflestirildi !"); sleep(5); printf("\n\n"); resetColor(); clearScreen();
    }
    fclose(ptr);  // Dosyayi kapatma
}

void musteriListele(){
    clearScreen();
    setColor(14);
    printFrame("             Musteri Listeleme Ekrani             "); printf("\n\n");
    resetColor();
    int sayac=0; char tercih;
    FILE * ptr= fopen("musteriler.dat","r+b");	// Musteri bilgilerini içeren dosyayi acma
    musteri m1;

	setColor(12); printCentered("%-10s%-15s%-30s%-30s%-20s%-30s%-10s%-10s%-10s","Mst.NO","TC","AD-SOYAD","ADRES",
		"TELEFON","E-POSTA","BAKIYE","SIF-DURUM","HSP-DURUM"); resetColor(); printf("\n\n");

	// Dosyadan musteri bilgilerini okuma ve ekrana yazdirma
    while (fread ( &m1, sizeof(musteri), 1, ptr) !=NULL )
    {
        setColor(11); printCentered("%-10d%-15s%-30s%-30s%-20s%-30s%-10.2f%-10d%-10d",m1.musteriNo,m1.tc,m1.adSoyad,m1.adres,
		m1.telefon,m1.eposta,m1.tlBakiye,m1.sifreDurum,m1.HesapDurum);printf("\n"); resetColor();
        sayac++;
    }
    setColor(10); printf("\n\n\n\n"); printCentered("Musteri sayisi: %d",sayac); printf("\n\n"); resetColor();
    sleep(3);

    setColor(10);printf("\n\n\n"); printCentered("Ana Menuye Don ? (E) : "); resetColor();
    setColor(12); scanf(" %c",&tercih); resetColor(); printf("\n\n");
    getchar();
    if(tercih == 'e' || tercih == 'E'){
        fclose(ptr); clearScreen();
    }

}

int yetkiliMenu(){
    int secim;
    clearScreen(); setColor(14);
    printFrame("             SAYIN YETKILI ADMIN HOSGELDINIZ             "); printf("\n\n");
    resetColor();

    setColor(11);
    printCentered("1- MUSTERI EKLE"); printf("\n\n");
    printCentered("2- MUSTERI SIL"); printf("\n\n");
    printCentered("3- HESAP DONDUR"); printf("\n\n");
    printCentered("4- HESAP AKTIFLESTIR"); printf("\n\n");
    printCentered("5- MUSTERI LISTELE"); printf("\n\n");
    printCentered("6- KREDI KARTI ISLEMLERI"); printf("\n\n");
    printCentered("0- CIKIS"); printf("\n\n");
    resetColor();
    setColor(10);
    printCentered("Seciminiz : ");
    resetColor();
    setColor(12); scanf("%d", &secim); resetColor();
    return secim;
}

void yetkili(){
    int secim = yetkiliMenu();

    while (secim != 0)
    {
        switch (secim)
        {
        case 1: musteriEkle(); break;
        case 2: musteriSil(); break;
        case 3: hesapDondur(); break;
        case 4: hesapAktiflestir(); break;
        case 5: musteriListele(); break;
        case 6: yetkilikredikartiislemleri(); break;
        case 0: break;
        default:
           setColor(12); printCentered("Hatali secim  !"); resetColor();
            break;
        }
        secim = yetkiliMenu();
    }
    clearScreen();
    setColor(10); printf("\n\n\n\n"); printCentered("ADMIN PANELINDEN CIKIS YAPILDI..."); printf("\n\n"); resetColor(); sleep(3);
}

void yetkilikredikartiislemleri() {
    clearScreen();
    setColor(14);
    printFrame("             KREDI KARTI ISLEMLERI             ");
    resetColor();
    setColor(11);
    printCentered("1- Kredi karti tanimlama islemi"); printf("\n\n");
    printCentered("2- Kredi karti limit guncelleme islemi"); printf("\n\n");
    printCentered("3- Kredi karti kapatma islemi"); printf("\n\n"); resetColor();
    setColor(10); printCentered("Seciminiz : "); resetColor();

    int IslemSecim;
    setColor(12); scanf("%d", &IslemSecim); resetColor();

    switch (IslemSecim) {
        case 1: krediKartiTanimla(); break;
        case 2: krediLimitiGuncelleme(); break;
        case 3: krediKartiKapatma(); break;
        default:
            setColor(12); printCentered("Gecersiz secim"); printf("\n\n"); resetColor();
            break;
    }
}

void krediKartiTanimla(){
    krediKarti kart;
    krediKarti mevcutKart;

    printf("\n\n");
    setColor(10); printCentered("TC Kimlik No : "); resetColor();
    setColor(12); scanf("%s", kart.musteriTc); resetColor();

    // Kredi karti bilgilerini dosyadan kontrol etme
    FILE *dosya = fopen("kredikartlari.dat", "rb");
    if (dosya != NULL) {
        while (fread(&mevcutKart, sizeof(krediKarti), 1, dosya) == 1) {
            if (strcmp(mevcutKart.musteriTc, kart.musteriTc) == 0) {
                // Mevcut kart bulundu
                setColor(11);
                printf("\n\n"); sleep(1);
                printCentered("Bu TC Kimlik No'ya ait bir kredi karti sistemde kayitlidir : ");
                printf("\n\n"); sleep(1);
                printCentered("Kart No: %s", mevcutKart.kartNo); printf("\n");
                printCentered("Son Kullanma Tarihi: %s", mevcutKart.sonKullanma); printf("\n");
                printCentered("Guvenlik Kodu: %d", mevcutKart.guvenlikKodu); printf("\n");
                resetColor();
                fclose(dosya);
                return;
            }
        }
        fclose(dosya);
    }
    srand(time(0));	 // Rastgele sayi uretebilmek icin zaman bazli bir deger olusturma

    // Kart numarasini rastgele olusturma
    sprintf(kart.kartNo, "5%09lu", rand() % 1000000000);

    // Son kullanma tarihini rastgele olusturma
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(kart.sonKullanma, "%02d/%d", tm.tm_mon + 1, tm.tm_year + 1900 + 3); // 3 yil sonra son kullanma

    // Güvenlik kodunu rastgele olusturma
    kart.guvenlikKodu = rand() % 900 + 100;

    // Kredi karti bilgilerini ekrana yazdirma
    printf("\n\n");
    setColor(11); sleep(1);
    printCentered("Kredi Karti Bilgileri : ");  printf("\n");
    printCentered("Kart No : %s\n", kart.kartNo);  printf("\n");
    printCentered("Son Kullanma Tarihi : %s", kart.sonKullanma);  printf("\n");
    printCentered("Guvenlik Kodu : %d", kart.guvenlikKodu);  printf("\n");
    resetColor();

    // Kredi karti bilgilerini dosyaya kaydetme
    dosya = fopen("kredikartlari.dat", "ab");
    if (dosya == NULL) {
        setColor(12); sleep(1);  printCentered("Dosya olusturulamadi!");  printf("\n"); resetColor();
        return;
    }
    fwrite(&kart, sizeof(krediKarti), 1, dosya);
    fclose(dosya);
}

void krediLimitiGuncelleme(){
    char tc[20];
    double yeniLimit;
    krediKarti kart;
    int bulundu = 0;

    setColor(10); printCentered("Musteri TC Kimlik No : "); resetColor();
    setColor(12); scanf("%s", tc); resetColor();

    // Mü?teri dosyas?n? açma
    FILE *musteriDosya = fopen("musteriler.dat", "rb+");
    if (musteriDosya == NULL) {
        setColor(12); printCentered("Musteri bilgileri dosyasi acilamadi!"); resetColor();
        return;
    }

    musteri m;

    // TC numarasina ait musteriyi bulma
    while (fread(&m, sizeof(musteri), 1, musteriDosya) == 1) {
        if (strcmp(m.tc, tc) == 0) {
            bulundu = 1;
            break;
        }
    }

    fclose(musteriDosya);

    if (!bulundu) {
        setColor(12); printCentered("Belirtilen TC numarasina ait musteri bulunamadi!"); printf("\n\n"); resetColor();
        return;
    }

    // Musteriye ait kredi kartlarinin dosyasini acma
    FILE *kartDosya = fopen("kredikartlari.dat", "rb+");
    if (kartDosya == NULL) {
        setColor(12); printCentered("Kredi karti bilgileri dosyasi acilamadi!"); printf("\n\n"); resetColor();
        return;
    }



    // TC numarasina ait kredi kartlarini bulma ve listeleme
    while (fread(&kart, sizeof(krediKarti), 1, kartDosya) == 1) {
        if (strcmp(kart.musteriTc, tc) == 0) {
            setColor(11); printf("\n\n"); printCentered("Musteriye ait kredi kartlari ve limitler : "); printf("\n\n"); resetColor();
            setColor(11); printCentered("Kart No: %s Limit: %.2lf", kart.kartNo, kart.limit); printf("\n\n"); resetColor();
            bulundu = 1;

            // Kullanilabilir limit kontrolu ve atanmasi
            if (kart.kullanilabilirLimit == 0) {
                kart.kullanilabilirLimit = kart.limit;
            }

            // Yeni limitin atanmasi
           printf("\n");
           setColor(10);  printCentered("Yeni Limit: "); resetColor();
           setColor(12);  scanf("%lf", &yeniLimit); resetColor();

            kart.limit = yeniLimit;
            kart.kullanilabilirLimit = yeniLimit;

            // Dosyaya yazma ve guncelleme
            fseek(kartDosya, -sizeof(krediKarti), SEEK_CUR);
            fwrite(&kart, sizeof(krediKarti), 1, kartDosya);
        }
    }

    fclose(kartDosya);

    if (!bulundu) {
        setColor(12); printCentered("Belirtilen TC numarasina ait kredi karti bulunamadi!"); printf("\n\n"); resetColor();
        return;
    }

    setColor(10); printCentered("Belirtilen TC numarasina ait kredi kartlarinin limitleri guncellendi!"); printf("\n\n"); resetColor();
}

void krediKartiKapatma(){
    clearScreen();
    setColor(14);
    printFrame("               KREDI KARTI KAPATMA ISLEMI               "); printf("\n\n");
    resetColor();

    // Kredi kartlar?n? tutan dosyay? açma
    FILE *dosya = fopen("kredikartlari.dat", "rb+");
    if (dosya == NULL) {
        setColor(12); printCentered("Kredi karti bilgileri dosyasi acilamadi!"); printf("\n\n"); resetColor();
        return;
    }

    krediKarti kart;
    char tcNo[20];

    setColor(10); printCentered("TC Kimlik No : "); resetColor();
    setColor(12); scanf("%s", tcNo); resetColor();

    int bulundu = 0;

    // TC numaras?na ait kredi kartlar?n? listeleme
    rewind(dosya);
    while (fread(&kart, sizeof(krediKarti), 1, dosya) == 1) {
        if (strcmp(kart.musteriTc, tcNo) == 0) {
            setColor(6); printCentered("Kart No: %s Limit: %.2lf", kart.kartNo, kart.limit); printf("\n\n"); resetColor();
            bulundu = 1;
        }
    }

    if (!bulundu) {
        setColor(12); printCentered("Belirtilen TC numarasina ait kredi karti bulunamadi!"); printf("\n\n"); resetColor();
        fclose(dosya);
        return;
    }

    // Hangi kredi kart?n?n kapat?lmas?n? istedi?ini kullan?c?dan al?nmas?
    char kapatilacakKartNo[20];
    setColor(12); printCentered("Kapatmak istediginiz kredi karti numarasini girin : "); printf("\n\n"); resetColor();
    scanf("%s", kapatilacakKartNo);

    // Dosyadaki kartlar? güncelleme ve pasife alma
    FILE *geciciDosya = fopen("gecici.dat", "wb");
    rewind(dosya);
    while (fread(&kart, sizeof(krediKarti), 1, dosya) == 1) {
        if (strcmp(kart.kartNo, kapatilacakKartNo) == 0 && strcmp(kart.musteriTc, tcNo) == 0) {
            setColor(10);  printCentered("Kart No: %s kapatildi.", kart.kartNo); printf("\n\n"); resetColor();
            continue;
        }
        fwrite(&kart, sizeof(krediKarti), 1, geciciDosya);
    }

    fclose(dosya);
    fclose(geciciDosya);
    remove("kredikartlari.dat");
    rename("gecici.dat", "kredikartlari.dat");
}

void yetkiliGiris(){
    clearScreen();
    setColor(14);
    printFrame("               YETKILI GIRIS EKRANI               "); printf("\n\n");
    resetColor();
    char tc[20];
    int parola;

    setColor(10); printCentered("TC : "); resetColor();
    setColor(12); scanf("%s", tc); resetColor(); printf("\n");
    setColor(10); printCentered("PAROLA : "); resetColor();
    setColor(12); scanf("%d", &parola); resetColor();

	// Girilen TC kimlik numaras? ve parolay? kontrol etme
    if (strcmp(tc, "admin") == 0 && parola == 123)
        yetkili();
    else
        setColor(12); printCentered("Hatali giris denemesi  !"); printf("\n\n"); resetColor();
}

void hareketKaydet(musteri m1, char * islemPtr, double tutar){
    hareket h1;
    h1.musteriNo = m1.musteriNo;
    strcpy( h1.adSoyad, m1.adSoyad );
    strcpy( h1.islem, islemPtr );
    strcpy( h1.sube, "Gelisim Sb.");
    h1.tarih = time(0);
    h1.hareketTutar = (tutar > 0) ? tutar : 0;

    char dosyaAdi[20];

    strcpy( dosyaAdi, m1.tc ) ;
    strcat( dosyaAdi,".dat" );
    FILE *ptr= fopen( dosyaAdi, "a+b");
    fwrite( &h1, sizeof(hareket), 1, ptr );
    fclose(ptr);
}

void bakiyeGuncelle(musteri m1, float para,int dovizSecim){
    FILE *ptr = fopen("musteriler.dat", "r+b");
    int sayac=0;
    musteri m2;
    while(fread( &m2, sizeof(musteri), 1, ptr) !=NULL )
    {
        if( strcmp( m1.tc, m2.tc) ==0  )
            break;

            sayac++;
    }
    rewind(ptr);

   if(dovizSecim==1){
    m2.usdBakiye += para;
   	scanf("usd bakiyem %f",&m2.usdBakiye );
   } else if(dovizSecim==2){
   	m2.euroBakiye += para;
   	scanf("euro bakiyem %f",&m2.tlBakiye );
   } else if(dovizSecim==3){
   m2.tlBakiye += para;
   	scanf("tl bakiyem %f",&m2.tlBakiye );
   } else{
   	setColor(12); printCentered("Bakiye Guncellenemedi..."); printf("\n\n"); resetColor();
   }

    fseek( ptr, (sayac) * sizeof(musteri), 0 );
    fwrite ( &m2, sizeof(musteri), 1, ptr);
    fclose(ptr);

}

void paraCek(musteri m1 ){
    clearScreen();
    setColor(14);
    printFrame("               PARA CEKME EKRANI               "); printf("\n\n");
    resetColor();
    float cekilecekTutar;
    setColor(10); printCentered("Cekmek Istediginiz Tutar : "); resetColor();
	setColor(12); scanf("%f",&cekilecekTutar ); resetColor();

    musteri m2;
    FILE *ptr = fopen("musteriler.dat", "r+b");
    while( fread( &m2, sizeof(musteri), 1, ptr) !=NULL )
    {
        if( strcmp( m1.tc, m2.tc) ==0  )
            break;
    }
    fclose(ptr);
    if( cekilecekTutar > m2.tlBakiye ){
        setColor(12); printCentered("Yetersiz Bakiye"); printf("\n\n"); resetColor();
        setColor(10); sleep(1); printCentered("Ana Menuye Aktariliyorsunuz..."); printf("\n\n"); resetColor(); sleep(3);
        clearScreen();
    }
    else
    {
        bakiyeGuncelle(m2, -cekilecekTutar,3);
        hareketKaydet(m2, "para cekme",cekilecekTutar);
        setColor(10);
        printf("\n\n"); printCentered("Islem Basarili"); printf("\n\n"); sleep(1);
        printCentered("Hesabinizdan %.2f TL para cekilmistir",cekilecekTutar); printf("\n\n");
        sleep(1);
        printCentered("Ana Menuye Aktariliyorsunuz..."); printf("\n\n");
        resetColor();
        sleep(3);
        clearScreen();
    }
}

void paraYatir(musteri m1 ){
    clearScreen();
    setColor(14);
    printFrame("             PARA YATIRMA EKRANI             "); printf("\n\n");
    resetColor();
    float yatirilacakTutar;

    setColor(10); printCentered("Yatirmak Istediginiz Tutar : "); resetColor();
	setColor(12); scanf("%f",&yatirilacakTutar ); resetColor();

    musteri m2;
    FILE *ptr = fopen("musteriler.dat", "r+b");	// Musteri bilgilerini içeren dosyayi acma ve kontrol
    while(fread( &m2, sizeof(musteri), 1, ptr) !=NULL )
    {
        if( strcmp( m1.tc, m2.tc) ==0  )
            break;
    }
    fclose(ptr);
        // Bakiyeye yatirilacak tutari ekleyip, hareket kaydetme fonksiyonlarini çagirma
        bakiyeGuncelle(m2, yatirilacakTutar,3);
        hareketKaydet(m2, "para Yatirma",yatirilacakTutar);
        setColor(10);
        printf("\n\n");printCentered("Para yatirma islemi basarili"); printf("\n\n");
        sleep(1);
        printCentered("Ana Menuye Aktariliyorsunuz..."); printf("\n\n");
        resetColor();
        sleep(3);
        clearScreen();
}

void havaleEFT(musteri m1 ){
    clearScreen();
    setColor(14);
    printFrame("             HAVALE/EFT ISLEMLERI             "); printf("\n\n");
    resetColor();
    float havaleUcreti;
    int hedefHesapNo;
    char hedefAdSoyad[20];
    char birlesik_metin[100];
    char birlesik_metin2[100];

    // Kullanicidan hedef hesap numarasi, ad soyad ve havale ücreti degerlerini alma
    setColor(10); printCentered("Hesap No : "); resetColor();
    setColor(12); scanf("%d", &hedefHesapNo); resetColor();
    setColor(10); printCentered("Ad Soyad : "); resetColor();
    setColor(12); scanf(" %[^\n]s", hedefAdSoyad ); resetColor();
    setColor(10); printCentered("Ucret (TL) : "); resetColor();
    setColor(12); scanf("%f",&havaleUcreti ); resetColor();
    musteri m2;
    int sonuc=0;
    FILE *ptr = fopen("musteriler.dat", "r+b");	// Musteri bilgilerini iceren dosyayi acma ve kontrol
    while( fread( &m2, sizeof(musteri), 1, ptr) !=NULL )
    {
        if( hedefHesapNo == m2.musteriNo && strcmp( hedefAdSoyad, m2.adSoyad ) ==0  )
        {
            sonuc=1;
             break;	// Hedef musteriyi buldugumuzda donguden çikma
        }
    }
    fclose(ptr);
	// Gonderen musteriyi bulma
    ptr = fopen("musteriler.dat", "r+b");
    if (sonuc==0)
    {
        setColor(12); printf("\n\n"); printCentered("Alici hesap numarasi veya ad soyad hatali !"); printf("\n\n"); resetColor();
        setColor(10); sleep(1); printCentered("Ana Menuye Aktariliyorsunuz..."); printf("\n\n"); sleep(3); resetColor();
        clearScreen();
        return;
    }

    musteri m3;
    int sonuc2=0;
    while(fread( &m3, sizeof(musteri), 1, ptr) !=NULL )
    {
        if( strcmp( m1.tc, m3.tc ) ==0 )
        {
            sonuc2=1;
            break;
        }
    }
    fclose(ptr);
    if( havaleUcreti  > m3.tlBakiye )

    {
        setColor(12); printf("\n\n"); printCentered("Yetersiz bakiye !"); printf("\n\n"); resetColor();
        sleep(1); printCentered("Ana Menuye Aktariliyorsunuz..."); printf("\n\n"); sleep(3);
        clearScreen();
    }
    else
    {
        bakiyeGuncelle(m3, -havaleUcreti, 3 );
        bakiyeGuncelle(m2, havaleUcreti, 3 );

		sprintf(birlesik_metin, "%s adina Havale/EFT gonderildi", hedefAdSoyad);
		hareketKaydet(m3, birlesik_metin, havaleUcreti);

		sprintf(birlesik_metin, "%s size Havale/EFT gonderdi", m2.adSoyad);
		hareketKaydet(m2, birlesik_metin, havaleUcreti);

        sleep(1);
        setColor(10);
        printf("\n\n"); printCentered("Havale/EFT islemi basarili"); printf("\n\n");
        sleep(1);
        printCentered("Ana Menuye Aktariliyorsunuz..."); printf("\n\n");
        resetColor();
        sleep(3);
        clearScreen();
    }
}

void dovizAl(musteri m1){
    clearScreen(); setColor(14);
    printFrame("             DOVIZ ALIM EKRANI             "); printf("\n\n"); resetColor();

    setColor(11);
    printCentered("1- USD AL"); printf("\n\n");
    printCentered("2- EURO AL"); printf("\n\n"); resetColor();
    setColor(10); printCentered("Seciminiz : "); resetColor();

    char birlesik_metin[100]; char birlesik_metin2[100]; int dovizSecim; float miktar,tutar;
    float dolarKuru = 32.0; float euroKuru = 35.0;

    setColor(12); scanf("%d", &dovizSecim); resetColor(); clearScreen();

    setColor(11); printf("\n\n");
    if (dovizSecim == 1) {
        printCentered("Dolar Kuru : %.2f",dolarKuru); printf("\n\n");
    } else if (dovizSecim == 2) {
    printCentered("Euro Kuru : %.2f",euroKuru); printf("\n\n"); resetColor();
    }else{
    }

    setColor(10); printCentered("Doviz almak istediginiz miktari girin : "); resetColor();

    setColor(12); scanf("%f", &miktar); resetColor();

    if (dovizSecim == 1) {
    	// Dolar alim islemi
        if (miktar * dolarKuru > m1.tlBakiye) { // Musterinin bakiye kontrolu saglanmaktadir
            setColor(12); printCentered("Yetersiz TL Bakiye! USD alim islemi gerceklestirilemedi"); printf("\n\n"); resetColor();
        } else {
        	// Dolar alim islemi gerceklestirme
            tutar=miktar*dolarKuru; // Girilen Dolar miktari ile Dolar kuru carpilarak tl karsiligi bulunur
            m1.usdBakiye += miktar; // Musteri bakiyesine Dolar miktari eklenir
            m1.tlBakiye -= tutar; // Musteri bakiyesinden TL tutari dusulur
            bakiyeGuncelle(m1, -tutar, 3);  // tl doviz id : 3
            bakiyeGuncelle(m1, miktar, dovizSecim); // Musteri bakiyesine Dolar tutari eklenir

            sprintf(birlesik_metin, "USD alim yapildi - %.2f USD", miktar);
            hareketKaydet(m1, birlesik_metin, tutar);

            sleep(1); setColor(10); printf("\n\n");
			printCentered("Doviz alim islemi tamamlandi."); printf("\n\n");
			printCentered("Alinan doviz miktari: %.2f USD (%.2f TL)", miktar,tutar);

            sleep(2); printCentered("Ana Menuye Aktariliyorsunuz..."); printf("\n\n");
            resetColor(); sleep(4); clearScreen();
        }
    } else if (dovizSecim == 2) {
        if (miktar * euroKuru > m1.tlBakiye) { // Musterinin bakiye kontrolu saglanmaktadir
            setColor(12); printCentered("Yetersiz TL Bakiye! EURO alim islemi gerceklestirilemedi"); printf("\n\n"); resetColor();
        } else {	// Euro alim islemi gerceklestirme
			tutar=miktar*euroKuru;	// Girilen Euro miktari ile Euro kuru çarpilarak tl karsiligi bulunur
			m1.euroBakiye += miktar;	// Musteri bakiyesine Euro miktari eklenir
			m1.tlBakiye -= tutar;	// Musteri bakiyesinden TL tutari dusulur
			bakiyeGuncelle(m1, -tutar, 3);	// tl doviz id : 3
			bakiyeGuncelle(m1, miktar, dovizSecim);	// Musteri bakiyesine Euro tutari eklenir

            sprintf(birlesik_metin, "Euro alim yapildi - %.2f EURO", miktar);
            hareketKaydet(m1, birlesik_metin, tutar);

			sleep(1); setColor(10); printf("\n\n");
			printCentered("Doviz alim islemi tamamlandi."); printf("\n");
			printCentered("Alinan doviz miktari: %.2f EURO (%.2f TL)", miktar,tutar); printf("\n\n");
			sleep(1); printCentered("Ana Menuye Aktariliyorsunuz....");
			sleep(4); resetColor(); clearScreen();
        }
    } else {
       setColor(12);  printCentered("Hatali doviz cinsi secimi"); printf("\n\n"); resetColor();
    }
}

void dovizSat(musteri m1){
    clearScreen();
    setColor(14);
    printFrame("             DOVIZ SATIS EKRANI             "); printf("\n\n");
    resetColor();
    setColor(11);
    printCentered("1: USD SAT"); printf("\n\n");
    printCentered("2: EURO SAT"); printf("\n\n"); resetColor();
    setColor(10); printCentered("Seciminiz   : "); resetColor();

    int dovizSecim;
    setColor(12); scanf("%d", &dovizSecim); resetColor(); printf("\n\n");
    clearScreen();

    float dolarKuru = 32.0;
    float euroKuru = 35.0;

    float miktar, tutar;

    setColor(11); printf("\n\n");
    if (dovizSecim == 1) {
        printCentered("Dolar Kuru : %.2f",dolarKuru); printf("\n\n");
    } else if (dovizSecim == 2) {
       printCentered("Euro Kuru : %.2f",euroKuru); printf("\n\n");
    } else {
    }

    resetColor();
    setColor(10); printCentered("Doviz satmak istediginiz miktari girin : "); resetColor();

    setColor(12); scanf("%f", &miktar); resetColor();

    char birlesik_metin[100]; char birlesik_metin2[100];

    if (dovizSecim == 1) {
    	// Dolar satis islemi
        if (miktar > m1.usdBakiye) { // Musterinin bakiye kontrolu saglanmaktadir
            setColor(12); printCentered("Yetersiz USD Bakiye! USD satis islemi gerceklestirilemedi"); printf("\n\n"); resetColor();
        } else {
			tutar = miktar * dolarKuru;	// Girilen Usd miktari ile Usd kuru carpilarak TL karsiligi bulunur
			m1.usdBakiye -= miktar;	// Musteri bakiyesinden usd tutari dusulur
			m1.tlBakiye += tutar;	// Musteri bakiyesine tl tutari eklenir
			bakiyeGuncelle(m1, tutar, 3);	// tl doviz id : 3
			bakiyeGuncelle(m1, -miktar, dovizSecim);	// Musteri bakiyesinden usd miktari dusulur

            sprintf(birlesik_metin, "USD satis yapildi - %.2f USD", miktar);
            hareketKaydet(m1, birlesik_metin, tutar);
			sleep(1); setColor(10); printf("\n\n");
			printCentered("Doviz satis islemi tamamlandi."); printf("\n");
			printCentered("Satilan doviz miktari: %.2f USD (%.2f TL)", miktar,tutar); printf("\n\n");
			sleep(2);
			printCentered("Ana Menuye Aktariliyorsunuz...."); printf("\n"); resetColor();
			sleep(4);
			clearScreen();
        }
    } else if (dovizSecim == 2) {	// Euro satis islemi
        if (miktar > m1.euroBakiye) {  // Musterinin bakiye kontrolu saglanmaktadir
            setColor(12); printCentered("Yetersiz EURO Bakiye! EURO satis islemi gerceklestirilemedi..");  printf("\n\n"); resetColor();
        } else {
			tutar = miktar * euroKuru;	// Girilen Euro miktari ile Euro kuru carpilarak tl karsiligi bulunur
			m1.euroBakiye -= miktar;	// Musteri bakiyesinden euro tutari dusulur
			m1.tlBakiye += tutar;	// Musteri bakiyesine tl tutari eklenir
			bakiyeGuncelle(m1, tutar, 3);	// tl doviz id : 3
			bakiyeGuncelle(m1, -miktar, dovizSecim);	// Musteri bakiyesinden euro miktari dusulur

            sprintf(birlesik_metin, "Euro satis yapildi - %.2f USD", miktar);
            hareketKaydet(m1, birlesik_metin, tutar);
			sleep(1); setColor(10); printf("\n\n");
			printCentered("Doviz satis islemi tamamlandi.");printf("\n");
			printCentered("Satilan doviz miktari: %.2f EURO (%.2f TL)", miktar);   printf("\n\n");
			sleep(2);
			printCentered("Ana Menuye Aktariliyorsunuz..."); printf("\n"); resetColor();
			sleep(4);
			clearScreen();
        }
    } else {
        setColor(12); printCentered("Hatali doviz cinsi secimi"); printf("\n"); resetColor();
    }
}

void dovizAlSat(musteri m1){
    clearScreen();
    setColor(14);
    printFrame("             DOVIZ ALIM/SATIM EKRANI             "); printf("\n\n");
    resetColor();
    setColor(11);
    printCentered("1- Doviz Alis Islemi", m1.musteriNo); printf("\n\n");
    printCentered("2- Doviz Satis Islemi", m1.musteriNo); printf("\n\n"); resetColor();
    setColor(10); printCentered("Seciminiz   : ");
    resetColor();

    int dovizIslemSecim;
    setColor(12); scanf("%d", &dovizIslemSecim); resetColor();

    if (dovizIslemSecim == 1) {
        dovizAl(m1);
    } else if (dovizIslemSecim == 2) {
        dovizSat(m1);
    } else {
       setColor(12);  printCentered("Gecersiz secim.."); printf("\n"); sleep(3); resetColor(); clearScreen();
    }
}

faturaIslemleri(musteri m1){
	char telno[10];
	char faturano[7];
    int secim, fatura;

	// Fatura bilgilerini iceren dosyayi acma ve kapatma islemleri
    FILE *faturabilgileri;
    faturabilgileri = fopen("faturabilgileri.dat", "a");
    fclose(faturabilgileri);
    faturabilgileri = fopen("faturabilgileri.dat", "r");
    fclose(faturabilgileri);

    clearScreen();
    setColor(14);
    printFrame("             FATURA ODEME EKRANI             "); printf("\n\n");
    resetColor();

    setColor(11);
	printCentered("1. Telekomunikasyon Faturasi"); printf("\n\n");
    printCentered("2. Elektrik-Su-Dogalgaz Faturasi"); printf("\n\n"); resetColor();
    setColor(10); printCentered("Seciminiz : "); resetColor();

    int faturaSecim; int odemeturuSecim; int odemeOnayla;
    setColor(12); scanf("%d", &faturaSecim); resetColor(); printf("\n\n");

    if(faturaSecim==1)
	{ // Telekomunikasyon Faturasi odeme islemleri
        setColor(11); printCentered("Odemesini yapmak istediginiz telefon numarasini basinda '0' olmadan giriniz"); printf("\n\n"); resetColor();
            while (1)
            {
             setColor(10); printCentered("Telefon Numarasi : "); resetColor();
             setColor(12); scanf("%s", &telno); resetColor();
               if (telno[0] == '0')
                 {
                   setColor(12); printCentered("Lutfen basina '0' koymadan yaziniz.Ana menuye donmek icin '1'e basiniz"); printf("\n\n"); resetColor();
                 }
                   else if (telno[0] == '1' && strlen(telno) == 1)
                {	// telno uzunlugu 1 rakam ise ve rakam degeri 1 girildiyse ana menuye donulur
                   break;
                }
                    else if (strlen(telno) == 10)	// girilen telno uzunlugu 10 karakter ise fatura odeme islemine girilir
                {

                fatura = rand() % 70 + 20;	// Bu ifade 20 ile 89 (20 + 69) arasinda rastgele bir sayi elde etmek için kullanilir
              	setColor(10); printf("\n\n");
              	printCentered("%s numarali telefonun %d TL odemesi bulunmaktadir", telno, fatura); printf("\n\n"); resetColor();
              	setColor(11); printCentered("1- Fatura Ode", telno, fatura); printf("\n");
              	printCentered("2- Ana menuye don..", telno, fatura); printf("\n\n"); resetColor();
              	setColor(10); printCentered("Seciminiz : "); resetColor();
           		setColor(12); scanf("%d", &secim); resetColor();
                if (secim == 1)
                {
                    setColor(12); printf("\n\n"); printCentered("Odeme turunu seciniz"); printf("\n\n"); resetColor();
                    setColor(11); printCentered("1- Banka hesabimdan Ode", odemeturuSecim); printf("\n");
                	printCentered("2- Kredi karti ile Ode", odemeturuSecim);  printf("\n\n"); resetColor();
                	setColor(10); printCentered("Seciminiz : "); resetColor();
                	setColor(12); scanf("%d", &odemeturuSecim); resetColor();
            if(odemeturuSecim == 1){
                if (m1.tlBakiye < fatura) {
         		setColor(12); printf("\n\n");
         		printCentered("Belirtilen islemi yapmak icin yeterli bakiyeniz bulunmamaktadir"); printf("\n\n"); resetColor();	sleep(3);
            	break;
          		}
           else
          	{	// Fatura odeme islemleri
			faturabilgileri = fopen("faturabilgileri.dat", "a");
	        setColor(10); printf("\n\n"); printCentered("Odeme isleminiz gerceklestiriliyor..."); printf("\n"); resetColor();
        	m1.tlBakiye -= fatura;
        	bakiyeGuncelle(m1, -fatura, 3);	// Musteri bakiyesinden fatura tutari dusulur
        	hareketKaydet(m1, "Telekomunikasyon fatura odemesi",fatura);
        	fprintf(faturabilgileri, "%s\n", telno);
        	fclose(faturabilgileri);

        	sleep(2); setColor(10); printf("\n\n");
        	printCentered("Odeme isleminiz basariyla gerceklesmistir."); printf("\n\n"); resetColor();
        	sleep(3); setColor(10);
        	printCentered("Ana Menuye Yonlendiriliyorsunuz. Lutfen Bekleyin...");
        	printf("\n\n"); resetColor(); sleep(3); clearScreen();
        	break;
    		}
					} else if (odemeturuSecim == 2) {
                        FILE *kartDosya = fopen("kredikartlari.dat", "rb+");
                        if (kartDosya == NULL) {
                            setColor(12); printf("\n\n");
                            printCentered("Kredi karti bilgileri dosyasi acilamadi!"); printf("\n\n"); resetColor();
                            return;
                        }

                        krediKarti kart;
                        int bulundu = 0;
                        while (fread(&kart, sizeof(krediKarti), 1, kartDosya) == 1) {

                            if (strcmp(kart.musteriTc, m1.tc) == 0) {
                                printCentered("Kredi karti odeme islemlerine girdin 3");
                                if (kart.kullanilabilirLimit >= fatura) {
                                	faturabilgileri = fopen("faturabilgileri.dat", "a");
                                	kart.kullanilabilirLimit -= fatura;
                                	hareketKaydet(m1, "Telekomunikasyon fatura odemesi (kredi karti ile odeme)",fatura);
                                    fseek(kartDosya, -sizeof(krediKarti), SEEK_CUR);
                                    fwrite(&kart, sizeof(krediKarti), 1, kartDosya);
                                    bulundu = 1;
                                    fprintf(faturabilgileri, "%s\n", telno);
                                    fclose(faturabilgileri); printf("\n\n");
                                    setColor(10); printCentered("Odeme isleminiz gerceklestiriliyor..."); printf("\n\n"); resetColor();
                                    sleep(2); setColor(10);
                                    printCentered("Odeme isleminiz basariyla gerceklesmistir."); printf("\n\n"); resetColor();
                                    sleep(3); setColor(10);
                                    printCentered("Ana Menuye Yonlendiriliyoruniz. Lutfen Bekleyin..."); printf("\n\n"); resetColor();
        							sleep(3); clearScreen();
        							break;
                                } else {
                                    setColor(12); printf("\n\n"); printCentered("Kredi karti Kullanilabilir limit yetersiz!"); printf("\n\n"); resetColor();
                                }
                            }
                        }

                        fclose(kartDosya);
                        break;
                        if (!bulundu) {
                            setColor(12); printf("\n\n"); printCentered("Belirtilen TC numarasina ait kredi karti bulunamadi!"); printf("\n\n"); resetColor();
                        }
                    } else{
						setColor(12); printf("\n\n"); printCentered("Lutfen belirtilen araliklarda bir secim yapiniz"); printf("\n\n"); resetColor();
					}

        }
        else if (secim == 2)	//	secim 2 secildiginde donguden cikilir
        {
		break;
		}
         else
        {
        setColor(12); printf("\n\n"); printCentered("Lutfen belirtilen araliklarda bir secim yapiniz"); printf("\n\n"); resetColor();
        }
        }
         else
        {	// telno karakter uzunlugu 10 karakterden az ise hata mesaji gosterilir
        setColor(12); printf("\n\n");
        printCentered("Lutfen 10 karakterden olusan bir telefon numarasi giriniz.Ana menuye donmek icin '1'e basiniz"); printf("\n\n"); resetColor();
        }
    }


	} else if(faturaSecim==2)	// Elektrik-Su-Dogalgaz faturasi secildiyse
	{
		setColor(10); printf("\n\n");
		printCentered("Lutfen odemek istediginiz faturanin musteri no'sunu giriniz(Orn:A56845,G18632).Ana menuye donmek icin '1'e basiniz.");
		printf("\n\n"); resetColor();
            while (1)
            {
                setColor(12); printCentered(" "); scanf("%s", &faturano); resetColor();
				// telno uzunlugu 1 rakam ise ve rakam degeri 1 girildiyse ana menuye donulur
                if (faturano[0] == '1' && strlen(faturano) == 1)
                {
                    break;
                }
                //faturano basinda buyuk harfler ile A-Z araliginda harf girilmemisse hata mesaji gosterilir
                else if (!(faturano[0] >= 'A' && faturano[0] <= 'Z'))
                {
                    setColor(10); printCentered("Musteri no'sunun en basinda buyuk(A-Z) bir harf olmalidir.Tekrar deneyiniz veya '0'a basarak ana menuye donunuz"); printf("\n\n"); resetColor();
                }
                else if (strlen(faturano) != 6 || !(faturano[1] >= '0' && faturano[1] <= '9') || !(faturano[2] >= '0' && faturano[2] <= '9') ||
				!(faturano[3] >= '0' && faturano[3] <= '9') || !(faturano[4] >= '0' && faturano[4] <= '9') || !(faturano[5] >= '0' && faturano[5] <= '9'))
                {
                	// faturano degeri 6 karakter uzunlugunda ve son 5 karakter rakam degilse hata mesaji gosterilir.
                    setColor(12); printf("\n\n");
                    printCentered("Fatura no'su 1 harf ve 5 rakamdan olusmalidir.Tekrar deneyiniz veya '0'a basarak ana menuye donunuz"); printf("\n\n"); resetColor();
                }
                else
                {   // Elektrik-Su-Dogalgaz faturasini hesaplayarak kullaniciya gosterme
                    fatura = rand() % 200 + 50;	// Bu ifade, 50 ile 249 (199 + 50) arasinda rastgele bir sayi elde etmek icin kullanilir
                    setColor(10); printf("\n\n");
                    printCentered("%s numarali faturanin %d TL odemesi bulunmaktadir", faturano, fatura); printf("\n\n"); resetColor();
                    setColor(11); printCentered("1-Fatura Ode"); printf("\n");
                    printCentered("2-Ana menuye don"); printf("\n\n"); resetColor();
                    setColor(10); printCentered("Seciminiz : "); resetColor();
                    setColor(12); scanf("%d", &secim); resetColor();
                        if (secim == 1)	// 1 secildiginde fatura odeme isleminin tamamlanmasi icin isleme girer
                        {

                        		printf("\n\n"); setColor(12); printCentered("Odeme turunu seciniz"); printf("\n\n"); resetColor();
                        		setColor(11);printCentered("1- Banka hesabim ile Ode"); printf("\n");
                        		printCentered("2- Kredi karti ile Ode"); resetColor(); printf("\n\n");
                        		setColor(10); printCentered("Seciminiz : "); resetColor();
                	setColor(12); scanf("%d", &odemeturuSecim); resetColor();
                	if(odemeturuSecim == 1){
                            if (m1.tlBakiye < fatura)   // musteri bakiyesi fatura tutarindan dusuk ise hata mesaji ekrana yazdirilir
                            {
                                setColor(12); printCentered("Belirtilen islemi yapmak icin yeterli bakiyeniz bulunmamaktadir"); printf("\n\n"); resetColor();
                                sleep(3); break;
                            }
                            else
                            {  	// Fatura ödeme islemleri
                                faturabilgileri = fopen("faturabilgileri.dat", "a");
                                setColor(10); printf("\n\n"); printCentered("Odeme isleminiz gerceklestiriliyor..."); printf("\n"); resetColor();
                                m1.tlBakiye -= fatura;
                                bakiyeGuncelle(m1, -fatura, 3);	// musteri bakiyesinden fatura tutari dusulur
                                hareketKaydet(m1, ("Gider fatura odemesi"),fatura);
                                fprintf(faturabilgileri, "%s\n", faturano);
                                fclose(faturabilgileri);

                                setColor(10); sleep(2); printf("\n\n");
                                printCentered("Odeme isleminiz basariyla gerceklesmistir."); printf("\n\n"); sleep(3);
                                printCentered("Ana Menuye Yonlendiriliyorsunuz. Lutfen Bekleyin..."); sleep(6); printf("\n\n"); resetColor();
                                clearScreen();
                                break;
                            }
                        }
                        else if (odemeturuSecim == 2) {
                        FILE *kartDosya = fopen("kredikartlari.dat", "rb+");
                        if (kartDosya == NULL) {
                            setColor(12); printf("\n\n"); printCentered("Kredi karti bilgileri dosyasi acilamadi!"); printf("\n");
                            printCentered("Kredi kartiniz bulunmuyor"); printf("\n"); resetColor();
                            return;
                        }

                        krediKarti kart;
                        int bulundu = 0;

                        while (fread(&kart, sizeof(krediKarti), 1, kartDosya) == 1) {
                            if (strcmp(kart.musteriTc, m1.tc) == 0) {
                                if (kart.kullanilabilirLimit >= fatura) {
                                	faturabilgileri = fopen("faturabilgileri.dat", "a");
                                	kart.kullanilabilirLimit -= fatura;
                                	hareketKaydet(m1, "Gider fatura odemesi (kredi karti ile odeme)",fatura);
                                    fseek(kartDosya, -sizeof(krediKarti), SEEK_CUR);
                                    fwrite(&kart, sizeof(krediKarti), 1, kartDosya);
                                    bulundu = 1;
                                    fprintf(faturabilgileri, "%s\n", faturano);
                                    fclose(faturabilgileri);
                                    setColor(10); printf("\n\n");
                                    printCentered("Odeme isleminiz gerceklestiriliyor..."); printf("\n\n"); resetColor();
                                    sleep(2); setColor(10);
                                    printCentered("Odeme isleminiz basariyla gerceklesmistir."); printf("\n\n"); resetColor();
                                    sleep(3); setColor(10);
                                    printCentered("Ana Menuye Yonlendiriliyoruniz. Lutfen Bekleyin..."); printf("\n\n"); resetColor();
        							sleep(5); clearScreen();
        							break;

                                } else {
                                    setColor(12); printf("\n\n");
                                    printCentered("Kredi karti Kullanilabilir limit yetersiz!"); printf("\n\n"); resetColor();
                                }
                            }
                        }

                        fclose(kartDosya);
                        break;
                        if (!bulundu) {
                            setColor(12); printf("\n\n");
                            printCentered("Belirtilen TC numarasina ait kredi karti bulunamadi!"); printf("\n\n"); resetColor();
                        }
                    } else{
						setColor(12); printf("\n\n");
						printCentered("Lutfen belirtilen araliklarda bir secim yapiniz"); printf("\n\n"); resetColor();
					}
                    }
                        else if (secim == 2)	//	secim 2 secildiginde donguden çikilir
                        {
                            break;
                        }
                        else
                        {	// gecersiz bir deger girildiyse hata mesaji yaz
                            setColor(12); printf("\n\n");
                            printCentered("Lutfen belirtilen araliklarda bir secim yapiniz"); printf("\n\n"); resetColor();
                        }
                }
            }
	}else{
		setColor(12); printf("\n\n");
		printCentered("Gecersiz secim"); printf("\n\n"); resetColor(); // Gecersiz bir secim yapildiysa musteriye bilgi verilir
	}

}

void bakiyeSorgula(musteri m1 ){
    clearScreen();
    setColor(14);
    printFrame("             BAKIYE SORGULAMA EKRANI             "); printf("\n\n");
    resetColor();

    musteri m2; char tercih;
    FILE *ptr = fopen("musteriler.dat", "r+b");
    // Musteri bilgilerini dosyadan okuma
    while(fread( &m2, sizeof(musteri), 1, ptr) !=NULL )
    {
        if( strcmp( m1.tc, m2.tc) ==0  )
            break;
    }
    fclose(ptr);
    printf("\n");
    setColor(11);
    printCentered("TL Bakiyeniz   : %.2f TL",m2.tlBakiye); printf("\n\n");
    printCentered("USD Bakiyeniz  : %.2f USD", m2.usdBakiye); printf("\n\n");
    printCentered("EURO Bakiyeniz : %.2f EURO", m2.euroBakiye); printf("\n\n\n");
    resetColor();

    setColor(10); printCentered("Ana Menuye Don ? (E) : "); resetColor();
    setColor(12); scanf(" %c",&tercih); resetColor(); printf("\n\n");
    getchar();
    if(tercih == 'e' || tercih == 'E'){
        clearScreen();
    }
}

void sifreDegistir(musteri m1 ){
    clearScreen();
    setColor(14);
    printf("\n"); printFrame("               SIFRE DEGISTIRME EKRANI               "); printf("\n\n");
    resetColor();
    int eskiSifre,yeni, sifreTekrar;

    setColor(10); printCentered("Eski Sifre : "); resetColor();
    setColor(12); scanf("%d", &eskiSifre);  resetColor();
    setColor(10); printCentered("Yeni Sifre : ");  resetColor();
    setColor(12); scanf("%d", &yeni);  resetColor();
    setColor(10); printCentered("Yeni Sifre Tekrar : ");  resetColor();
    setColor(12); scanf("%d", &sifreTekrar);  resetColor();

    int sonuc=0, sayac =0 ;

    if ( m1.sifre== eskiSifre && yeni == sifreTekrar )
    {
        musteri m2;
        // Dosyayi okuma ve kullaniciyi bulma
        FILE *ptr= fopen("musteriler.dat","r+b");
        while( fread ( &m2, sizeof(musteri), 1, ptr ) !=NULL )
        {
            if ( strcmp( m1.tc, m2.tc ) ==0 )
            {
                sonuc=1;
                break;
            }
            sayac++;
        }
        if( sonuc==0)
        {
            // Kullanici bulunamazsa hata mesaji verme ve dosyayi kapatma
            setColor(12); printf("\n\n"); printCentered("Sifre degistirme isleminde hata olustu !");  printf("\n"); resetColor();
            clearScreen(); fclose(ptr);
        }
        else
        {
        	// Dosyanin basina donme ve kullanicinin sifresini degistirme
            rewind(ptr);
            fseek ( ptr, (sayac) * sizeof(musteri), 0 ) ;

            m2.sifre= yeni;
            fwrite ( &m2, sizeof(musteri), 1, ptr );
            fclose(ptr);
            setColor(10); printf("\n\n"); printCentered("Sifre degistirme isleminiz basarili."); printf("\n\n"); sleep(2);
            printCentered("Ana menuye yonlendiriliyorsunuz..."); printf("\n\n"); sleep(3); resetColor();
            clearScreen(); fclose(ptr);
        }
    }
    else
    {
        // Hata mesaji ekrana yazdirma
        setColor(12); printf("\n\n"); printCentered("Hatali sifre degistirme islemi !"); sleep(3); clearScreen(); printf("\n\n");
    }

}

void hareketRaporu(musteri m1 ){
    clearScreen(); setColor(14);
	printFrame("             HAREKET RAPOR EKRANI             "); printf("\n\n");
	resetColor();
	hareket h1; int sayac=0; double hareketTutar = 0; char dosyaAdi[20]; char tercih;

    strcpy( dosyaAdi, m1.tc ) ;
    strcat( dosyaAdi,".dat" );

    // Dosyayi acma ve okuma islemi
    FILE *ptr= fopen( dosyaAdi, "r+b");
    setColor(12);
    printCentered("%-10s%-30s%-20s%-60s%-15s%-30s\n", "Mst.No", "AD-SOYAD","SUBESI", "ISLEM", "TUTAR", "TARIH") ;
    resetColor();

    // ctime'dan gelen yeni sat?r karakterini kald?rma
        //char* tarih_str = ctime(&h1.tarih);
      //  tarih_str[strlen(tarih_str) - 1] = '\0';

    // Dosyadan hareketleri okuma ve ekrana yazdirma
    while ( fread( &h1, sizeof(hareket), 1, ptr ) != NULL )
    {
        setColor(11);
        printCentered("%-10d%-30s%-20s%-60s%-15.2f%-30s", h1.musteriNo, h1.adSoyad, h1.sube, h1.islem,h1.hareketTutar, ctime(&h1.tarih)); printf("\n");
        resetColor();
        sayac++;
    }
    fclose(ptr);
    setColor(10);
     printf("\n"); printCentered("Toplam islem sayisi : %d",sayac );  printf("\n");
    resetColor();

    setColor(10);printf("\n\n\n"); printCentered("Ana Menuye Don ? (E) : "); resetColor();
    setColor(12); scanf(" %c",&tercih); resetColor(); printf("\n\n");
    getchar();
    if(tercih == 'e' || tercih == 'E'){
        clearScreen();
    }
}

void krediKartiBilgileriniGoruntule(musteri m1) {
    clearScreen();
    setColor(14);
    printFrame("             KREDI KARTI BILGILERI             ");
    resetColor();
    // Kredi kartlarini tutan dosyayi acma
    FILE *dosya = fopen("kredikartlari.dat", "rb");
    if (dosya == NULL) {
            setColor(12);
        printCentered("Kredi karti bilgileri dosyasi acilamadi!"); printf("\n\n");
    resetColor();
        return;
    }

    krediKarti kart;
    int bulundu = 0; char tercih;
    // TC numarasina ait kredi kartlarini listeleme
    rewind(dosya);
    while (fread(&kart, sizeof(krediKarti), 1, dosya) == 1) {
        if (strcmp(kart.musteriTc, m1.tc) == 0) {
            setColor(11);
            printf("\n\n");
            printCentered("Kart No: %s\tSon Kullanma Tarihi: %s\tGuvenlik Kodu: %d\tLimit: %.2lf\t Kullanilabilir Limit: %.2lf\tSifre: %s", kart.kartNo, kart.sonKullanma, kart.guvenlikKodu, kart.limit, kart.kullanilabilirLimit, kart.sifre);
            resetColor();
            printf("\n\n\n\n");
			bulundu = 1;
        }
    }

    if (!bulundu) {
            setColor(11); printCentered("Kredi kartiniz bulunmamaktadir !"); printf("\n"); resetColor();
    }

    fclose(dosya);
    setColor(10);printf("\n\n\n"); printCentered("Ana Menuye Don ? (E) : "); resetColor();
    setColor(12); scanf(" %c",&tercih); resetColor(); printf("\n\n");
    getchar();
    if(tercih == 'e' || tercih == 'E'){
        clearScreen();
    }
}

void krediKartiBorcOdeme(musteri m1) {
    clearScreen();
    setColor(14);
    printFrame("                KREDI KARTI BILGILERI                "); printf("\n\n");
    resetColor();
    double kredikartiborc;
    int odemesecim;
    int odemeturuSecim;
    double kredikartiodenecekborc;

    // Kredi kartlarini tutan dosyayi acma
    FILE *kartDosya = fopen("kredikartlari.dat", "rb+");
    if (kartDosya == NULL) {
            setColor(12);
        printCentered("Kredi karti bilgileri dosyasi acilamadi!"); printf("\n\n");
    resetColor();
        return;
    }

    krediKarti kart;
    int bulundu = 0;
    // TC numarasina ait kredi kartlarini listeleme
    rewind(kartDosya);
    while (fread(&kart, sizeof(krediKarti), 1, kartDosya) == 1) {
        if (strcmp(kart.musteriTc, m1.tc) == 0) {
            bulundu = 1;
            setColor(6);
            printf("\n"); printCentered("Limit: %.2lf Kullanilabilir Limit: %.2lf", kart.limit, kart.kullanilabilirLimit); printf("\n\n");
            resetColor();
            if (kart.kullanilabilirLimit == kart.limit) {
                setColor(10);
                printCentered("Kredi karti borcunuz bulunmamaktadir! Odemelerinizi zamaninda yaptiginiz icin tesekkur ederiz");  printf("\n\n");
                resetColor();
                sleep(3);
                clearScreen();
                break;
            } else if (kart.kullanilabilirLimit < kart.limit) {
                kredikartiborc = kart.limit - kart.kullanilabilirLimit;
                setColor(6);
                printCentered("Kredi karti borcunuz: %.2lf", kredikartiborc);   printf("\n");
                resetColor(); setColor(11); printf("\n\n");
                printCentered("Kart borcunuzu odemek ister misiniz ?"); printf("\n");
                printCentered("1- Evet"); printf("\n");
                printCentered("2- Hayir. Ana menuye don."); printf("\n\n");
                resetColor();
                setColor(10); printCentered("Seciminiz : "); resetColor();
                setColor(12); scanf("%d", &odemesecim); resetColor(); printf("\n\n");
                if (odemesecim == 1) {
                    // Hesaptan ödeme islemi
                    setColor(11);
                    printCentered("Kart borcunuzu nasil odemek istersiniz ?"); printf("\n");
                    printCentered("1- Tamami"); printf("\n");
                    printCentered("2- Girilecek tutar ile."); printf("\n\n");
                    resetColor();
                    setColor(10); printCentered("Seciminiz : "); resetColor();
                    setColor(12); scanf("%d", &odemeturuSecim); resetColor(); printf("\n\n");
                    if (odemeturuSecim == 1) {
                        if (m1.tlBakiye < kredikartiborc) { // musteri bakiyesi fatura tutarindan dusuk ise musteriye hata mesaji gosterilir
                            setColor(12); printf("\n\n"); printCentered("Belirtilen islemi yapmak icin yeterli bakiyeniz bulunmamaktadir");  printf("\n\n"); resetColor();
                            sleep(2);
                            break;
                        } else {
                            setColor(10); printf("\n\n"); printCentered("Odeme isleminiz gerceklestiriliyor..."); printf("\n\n");  resetColor();
                            m1.tlBakiye -= kredikartiborc;
                            kart.kullanilabilirLimit += kredikartiborc;
                            bakiyeGuncelle(m1, -kredikartiborc, 3); // Musteri bakiyesinden kredi karti borcu tutari dusulur
                            fseek(kartDosya, -sizeof(krediKarti), SEEK_CUR);
                            fwrite(&kart, sizeof(krediKarti), 1, kartDosya);

                            hareketKaydet(m1, "Kredi karti borc odemesi", kredikartiborc);  // Kredi karti borcu odemesi hareket kaydina yazilir
                            sleep(2);
                            setColor(10); printCentered("Odeme isleminiz basariyla gerceklesmistir."); printf("\n\n"); resetColor();
                            sleep(3);
                            setColor(10); printCentered("Ana Menuye Yonlendiriliyorsunuz. Lutfen Bekleyin..."); printf("\n\n"); resetColor();
                            sleep(3);
                            clearScreen();
                            break;
                        }
                    } else if (odemeturuSecim == 2) {
                        setColor(10);
                        printCentered("Odeme yapmak istediginiz tutari giriniz."); resetColor();
                        setColor(12); scanf("%lf", &kredikartiodenecekborc); resetColor(); printf("\n\n");
                        if (m1.tlBakiye < kredikartiodenecekborc) { // musteri bakiyesi fatura tutarindan dusuk ise musteriye hata mesaji gösterilir
                            setColor(12); printf("\n\n"); printCentered("Belirtilen islemi yapmak icin yeterli bakiyeniz bulunmamaktadir"); printf("\n\n"); resetColor();
                            sleep(2);
                            break;
                        } else {
                            kredikartiborc = kart.limit - kart.kullanilabilirLimit;
                            if (kredikartiodenecekborc > kredikartiborc) {
                                setColor(12); printf("\n\n"); printCentered("Toplam borc tutarinizin uzerinde odeme yapamazsiniz..."); printf("\n"); resetColor();
                                sleep(3);
                                setColor(10); printf("\n\n"); printCentered("Ana Menuye Yonlendiriliyoruniz. Lutfen Bekleyin..."); printf("\n"); resetColor();
                                sleep(3);
                                clearScreen();
                                break;
                            } else {
                                setColor(10); printCentered("Odeme isleminiz gerceklestiriliyor..."); printf("\n\n"); resetColor();
                                m1.tlBakiye -= kredikartiodenecekborc;
                                kart.kullanilabilirLimit += kredikartiodenecekborc;
                                bakiyeGuncelle(m1, -kredikartiodenecekborc, 3); // Mü?teri bakiyesinden kredi karti borcu tutar? dü?ülür
                                fseek(kartDosya, -sizeof(krediKarti), SEEK_CUR);
                                fwrite(&kart, sizeof(krediKarti), 1, kartDosya);

                                hareketKaydet(m1, "Kredi karti borc odemesi", kredikartiodenecekborc); // Kredi karti borcu ödemesi hareket kaydina yazilir
                                sleep(2);
                                setColor(10); printCentered("Odeme isleminiz basariyla gerceklesmistir."); printf("\n\n"); resetColor();
                                sleep(3);
                                setColor(10); printCentered("Ana Menuye Yonlendiriliyoruniz. Lutfen Bekleyin..."); printf("\n\n"); resetColor();
                                sleep(3);
                                clearScreen();
                                break;
                            }
                        }
                    } else {
                        setColor(12); printCentered("Lutfen belirtilen araliklarda bir secim yapiniz"); printf("\n\n"); resetColor();
                    }
                    setColor(10); printCentered("Odeme islemleri burada yapilacak"); printf("\n\n"); resetColor();
                    sleep(3);
                    break;
                } else if (odemesecim == 2) {
                    break;
                } else {
                    setColor(12); printCentered("Lutfen belirtilen araliklarda bir secim yapiniz"); printf("\n\n"); resetColor();
                }
            } else {
                setColor(12); printCentered("Sistemde sorun olustu musteri hizmetleri ile iletisime geciniz !"); printf("\n\n"); resetColor();
                sleep(2);
                clearScreen();
                break;
            }
        }
    }

    if (!bulundu) {
        setColor(12); printCentered("Kredi kartiniz bulunmamaktadir !"); printf("\n\n"); resetColor();
    }

    fclose(kartDosya);
}

void krediKartiSifreBelirleme(musteri m1) {
    char eskiSifre[7];  // 6 karakter + null karakteri icin
    FILE *kartDosya = fopen("kredikartlari.dat", "rb+");
    if (kartDosya == NULL) {
        setColor(12);
        printCentered("Kredi karti bilgileri dosyasi acilamadi!"); printf("\n");
        resetColor();
        return;
    }

    krediKarti kart;
    int bulundu = 0;
    // TC numarasina ait kredi kartlarini listeleme
    rewind(kartDosya);
    while (fread(&kart, sizeof(krediKarti), 1, kartDosya) == 1) {
        if (strcmp(kart.musteriTc, m1.tc) == 0) {
            bulundu = 1;

            if (kart.sifre[0] != '\0') {  // Sifrenin tanimli olup olmadigini kontrol ediyoruz
            setColor(12);
            printf("\n\n"); printCentered("(gizli) Eski sifreniz : %s",kart.sifre);
            resetColor();
                setColor(10); printf("\n\n");
                printCentered("Eski sifrenizi giriniz: ");
                resetColor();
                setColor(12);
                scanf("%6s", eskiSifre);
                resetColor();
                if (strcmp(eskiSifre, kart.sifre) == 0) {
                    setColor(10);  printf("\n\n");
                    printCentered("Yeni sifreyi giriniz (maksimum 6 karakter) : ");
                    resetColor();
                    setColor(12);
                    scanf("%6s", kart.sifre);
                    resetColor();
                    fseek(kartDosya, -sizeof(krediKarti), SEEK_CUR);
                    fwrite(&kart, sizeof(krediKarti), 1, kartDosya);
                    setColor(11); printf("\n\n"); printCentered("Sifre basariyla tanimlandi/guncellendi."); printf("\n\n"); resetColor();
                    sleep(3);
                    clearScreen();
                    break;
                } else {
                    setColor(12); printCentered("Yanlis sifre girdiniz!"); resetColor();  printf("\n");
                }
            } else {
                setColor(12); printf("\n"); printCentered("Kart sifreniz tanimli degildir");   printf("\n");  resetColor(); sleep(1);
                setColor(10); printf("\n"); printCentered("Kartiniza sifre tanimlayin (maksimum 6 karakter) : "); resetColor();
                setColor(12); scanf("%6s", kart.sifre); resetColor();
                fseek(kartDosya, -sizeof(krediKarti), SEEK_CUR);
                fwrite(&kart, sizeof(krediKarti), 1, kartDosya);
                setColor(11); printCentered("Sifre basariyla tanimlandi/guncellendi."); printf("\n"); resetColor();
                sleep(3);
                clearScreen();
            }
        }
    }

    if (!bulundu) {
        setColor(12); printCentered("Kredi kartiniz bulunmamaktadir!"); printf("\n"); resetColor();
    }

    fclose(kartDosya);
}

void krediKartiIleOdeme(double tutar, musteri m1) {
    setColor(10);
    printf("\n\n"); printCentered("Kredi karti ile odeme yapilacak...");  printf("\n");
    resetColor();
    FILE *kartDosya = fopen("kredikartlari.dat", "rb+");
                        if (kartDosya == NULL) {
                            setColor(12);
                            printCentered("Kredi karti bilgileri dosyasi acilamadi!"); printf("\n");
                            resetColor();
                            return;
                        }

                        krediKarti kart;
                        int bulundu = 0;

                        while (fread(&kart, sizeof(krediKarti), 1, kartDosya) == 1) {
                            if (strcmp(kart.musteriTc, m1.tc) == 0) {
                                if (kart.kullanilabilirLimit >= tutar) {
                                	kart.kullanilabilirLimit -= tutar;
                                	hareketKaydet(m1, "Sigorta policeniz SEN SIGORTA tarafindan duzenlenmistir",tutar);
                                    fseek(kartDosya, -sizeof(krediKarti), SEEK_CUR);
                                    fwrite(&kart, sizeof(krediKarti), 1, kartDosya);
                                    bulundu = 1;
                                    setColor(10);
                                    printCentered("Odeme isleminiz gerceklestiriliyor...");  printf("\n");
                                    sleep(2);
                                    printCentered("Odeme isleminiz basariyla gerceklesmistir.");  printf("\n");
                                    printCentered("Sigorta policeniz SEN SIGORTA tarafindan duzenlenmistir.");  printf("\n");
                                    sleep(3);
        							printCentered("Ana Menuye Yonlendiriliyoruniz. Lutfen Bekleyin...");  printf("\n");
        							sleep(3);
        							resetColor();
        							clearScreen();
        							break;
                                } else {
                                    setColor(12);
                                    printCentered("Kredi karti Kullanilabilir limit yetersiz!"); printf("\n");
                                    resetColor();
                                    break;
                                }
                            }
                        }

                        fclose(kartDosya);

                        if (!bulundu) {
                            setColor(12);
                            printCentered("Belirtilen TC numarasina ait kredi karti bulunamadi!"); printf("\n"); resetColor();
                            sleep(3); setColor(12);
                            printCentered("Ana Menuye Yonlendiriliyoruniz. Lutfen Bekleyin...");  printf("\n");	sleep(3);
                            resetColor();
                        }
}

void bankaHesabileIleOdeme(double tutar, musteri m1) {
    setColor(10);
    printCentered("Banka hesabinizdan odeme yapiliyor..."); printf("\n\n");


    if (m1.tlBakiye >= tutar) { // Banka hesabinda yeterli bakiye kontrolu
        m1.tlBakiye -= tutar; // Odeme tutari banka hesabindan dusulur
        bakiyeGuncelle(m1, -tutar, 3);

        hareketKaydet(m1, "Sigorta policeniz SEN SIGORTA tarafindan duzenlenmistir", tutar);
        printCentered("Odeme isleminiz gerceklestiriliyor..."); printf("\n\n");
        sleep(1);
        printCentered("Odeme isleminiz basariyla gerceklesmistir."); printf("\n\n");
        sleep(1);
        printCentered("Sigorta policeniz SEN SIGORTA tarafindan duzenlenmistir."); printf("\n\n");
        sleep(1);
        printCentered("Yeni bakiyeniz: %.2lf TL\n", m1.tlBakiye); printf("\n\n");
        sleep(1);
        resetColor();
    } else {
        setColor(12);
        printCentered("Banka hesabinizda yeterli bakiye bulunmamaktadir!");  printf("\n");
        printCentered("Islem iptal edildi.");  printf("\n");
        resetColor();
    }
    setColor(10);
    printCentered("Ana Menuye Yonlendiriliyorsunuz. Lutfen Bekleyin...");  printf("\n");
    resetColor();
    sleep(3);
    clearScreen();
}

void aracSigortasiSecimi(musteri m1) {
    clearScreen(); setColor(14);
    printFrame("     ARAC SIGORTASI SECIMI     ");printf("\n\n");
    resetColor();
    char plaka[20]; char model[50];
    // Kullanicidan arac plakasi ve model bilgilerini aliyoruz
    setColor(10); printCentered("Arac Plakasi (27AHT06) : "); resetColor();
    setColor(12); scanf("%s", plaka); resetColor();

    setColor(10); printCentered("Arac Modeli (2024) : "); resetColor();
    setColor(12); scanf("%s", model); resetColor();

    srand(time(NULL));
    double sigortaBedeli = rand() % 4001 + 1000; // 1000-5000 arasinda rastgele bir tutar

    setColor(6); printf("\n\n");
    printCentered("Arac Sigorta Bedeli: %.2lf TL", sigortaBedeli);  printf("\n");
    printCentered("Sigorta gecerlilik suresi : 1 yil"); printf("\n\n");
    resetColor();

    setColor(12); printCentered("Odeme Secenekleri"); printf("\n"); resetColor();
    setColor(11);
    printCentered("1. Kredi Karti ile Odeme"); printf("\n");
    printCentered("2. Banka Hesabi ile Havale"); printf("\n"); resetColor();
    setColor(10);  printCentered("Seciminiz : "); resetColor();
    int odemeSecim;
    setColor(12); scanf("%d", &odemeSecim); resetColor();

    switch (odemeSecim) {
        case 1:
            krediKartiIleOdeme(sigortaBedeli, m1);
            break;
        case 2:
             bankaHesabileIleOdeme(sigortaBedeli, m1);
            break;
        default:
            setColor(12); printCentered("Gecersiz secim");printf("\n\n"); resetColor();
            break;
    }
}

void konutSigortasiSecimi(musteri m1) {
    clearScreen();
    setColor(14);
    printFrame("     KONUT SIGORTASI SECIMI     "); printf("\n\n");
    resetColor();
    char il[50];
    char ilce[50];
    int adaNo, parselNo;

    setColor(10); printCentered("Il Seciniz : "); resetColor();
    setColor(12); scanf("%s", il); resetColor();
    setColor(10); printCentered("Ilce Seciniz : "); resetColor();
    setColor(12); scanf("%s", ilce); resetColor();
    setColor(10); printCentered("Ada No : "); resetColor();
    setColor(12); scanf("%d", &adaNo); resetColor();
    setColor(10); printCentered("Parsel No : "); resetColor();
    setColor(12); scanf("%d", &parselNo); resetColor();

    srand(time(NULL));
    double sigortaBedeli = rand() % 4001 + 1000; // 1000-5000 aras?nda rastgele bir tutar

    printf("\n\n");
    setColor(6);
    printCentered("Konut Sigortasi Bedeli: %.2lf TL", sigortaBedeli); printf("\n");
    printCentered("Sigorta gecerlilik suresi : 1 yil"); printf("\n\n");
    resetColor();


    setColor(12); printCentered("ODEME SECENEKLERI "); printf("\n"); resetColor();
    setColor(11);
    printCentered("1. Banka Hesabi ile Havale");  printf("\n");
    printCentered("2. Kredi Karti ile Odeme");  printf("\n\n"); resetColor();
    setColor(10); printCentered("Seciminiz : "); resetColor();


    int odemeSecim;
    setColor(12);
    scanf("%d", &odemeSecim);
    resetColor();
    switch (odemeSecim) {
        case 1:
            bankaHesabileIleOdeme(sigortaBedeli, m1);
            break;
        case 2:
            krediKartiIleOdeme(sigortaBedeli, m1);
            break;
        default:
            setColor(12); printCentered("Gecersiz secim..."); printf("\n\n"); resetColor();
            break;
    }
}

void saglikSigortasiSecimi(musteri m1) {
    clearScreen(); setColor(14);
    printFrame("              SAGLIK SIGORTASI SECIMI              "); printf("\n\n");
    resetColor();
    char tc[12];  char adSoyad[50];
    setColor(10); printCentered("Sigorta yapilacak kisinin TC Kimlik Numarasi : "); resetColor();
    setColor(12); scanf("%s", tc); resetColor();
    setColor(10); printCentered("Sigorta yapilacak kisinin Adi ve Soyadi : "); resetColor();
    setColor(12); scanf(" %[^\n]s", adSoyad ); resetColor();

    srand(time(NULL));
    double sigortaBedeli = rand() % 4001 + 1000; // 1000-5000 arasinda rastgele bir tutar

    setColor(6); printf("\n\n");
    printCentered("Saglik Sigortasi Bedeli: %.2lf TL", sigortaBedeli); printf("\n");
    printCentered("Sigorta gecerlilik suresi : 1 yil"); printf("\n\n");
    resetColor();

    setColor(12); printCentered("ODEME SECENEKLERI");  printf("\n"); resetColor();
    setColor(11); printCentered("1. Banka Hesabi ile Havale");  printf("\n");
    printCentered("2. Kredi Karti ile Odeme");  printf("\n\n"); resetColor();
    setColor(10);printCentered("Seciminiz : "); resetColor();
    resetColor();

    int odemeSecim;
    setColor(12); scanf("%d", &odemeSecim); resetColor();

    switch (odemeSecim) {
        case 1:
            bankaHesabileIleOdeme(sigortaBedeli, m1);
            break;
        case 2:
            krediKartiIleOdeme(sigortaBedeli, m1);
            break;
        default:
            setColor(12); printCentered("Gecersiz secim"); printf("\n"); resetColor();
            break;
    }
}

int musteriMenu(){
    int secim;

    setColor(14);
    printf("\n\n"); printFrame("                     ANA MENU                     "); printf("\n\n");
    resetColor();

    setColor(11);
    printCentered("1- PARA CEK");printf("\n\n");
    printCentered("2- PARA YATIR");printf("\n\n");
    printCentered("3- HAVALE/ EFT");printf("\n\n");
    printCentered("4- DOVIZ ISLEMLERI");printf("\n\n");
    printCentered("5- FATURA ISLEMLERI");printf("\n\n");
    printCentered("6- BAKIYE SORGULAMA");printf("\n\n");
    printCentered("7- SIFRE DEGISTIRME");printf("\n\n");
    printCentered("8- HAREKET RAPORU");printf("\n\n");
    printCentered("9- KREDI KARTI ISLEMLERI");printf("\n\n");
    printCentered("10- SIGORTA POLICESI ISLEMLERI");printf("\n\n");
    printCentered("0- CIKIS");printf("\n\n");
    resetColor();

    setColor(10);
    printCentered("Seciminiz : ");
    resetColor();

    setColor(12);
    scanf(" %d", &secim);
    resetColor();
    return secim;

}

void musterimiz( musteri m1){
    clearScreen();
    printf("\n\n");
    setColor(6);
    printCentered("Sayin %s Hosgeldiniz",m1.adSoyad);printf("\n\n");
    printCentered("Musteri No : %d \t Bakiye : %.2f",m1.musteriNo,m1.tlBakiye);
    resetColor();

    int secim = musteriMenu();

    while (secim != 0)
    {
        switch (secim)
        {
        case 1: paraCek(m1); break;
        case 2: paraYatir(m1); break;
        case 3: havaleEFT(m1); break;
        case 4: dovizAlSat(m1); break;
        case 5: faturaIslemleri(m1); break;
        case 6: bakiyeSorgula(m1); break;
        case 7: sifreDegistir(m1); break;
        case 8: hareketRaporu(m1); break;
        case 9: musterikredikartiislemleri(m1); break;
        case 10: sigortapolicesiislemleri(m1); break;
        case 0:  break;
        default:

            setColor(12); printf("\n\n"); printCentered("Hatali secim  !");printf("\n\n");resetColor();
            break;
        }
        secim = musteriMenu();
    }

    clearScreen();
    sleep(0);
    printf("\n\n");
    setColor(12);
    printCentered("%s sistemden cikis yaptiniz...", m1.adSoyad );
    sleep(1);
    clearScreen();
    printf("\n\n"); printCentered("Ana Menuye Yonlendiriliyorsunuz..."); printf("\n\n"); sleep(3);
    resetColor();
}

void musterikredikartiislemleri(musteri m1) {
    clearScreen(); setColor(14);
    printFrame("                   KREDI KARTI ISLEMLERI                   "); resetColor(); printf("\n\n");
    setColor(11);
    printCentered("1- Kredi karti bilgilerini goruntule"); printf("\n\n");
    printCentered("2- Kredi karti borc sorgulama ve odeme"); printf("\n\n");
    printCentered("3- Kredi karti sifre guncelleme"); printf("\n\n");
    resetColor();

    setColor(10); printCentered("Seciminiz : "); resetColor();

    int IslemSecim;
    setColor(12); scanf("%d", &IslemSecim); resetColor();

    switch (IslemSecim) {
        case 1: krediKartiBilgileriniGoruntule(m1); break;
        case 2: krediKartiBorcOdeme(m1); break;
        case 3: krediKartiSifreBelirleme(m1); break;
        default:
            setColor(12); printCentered("Gecersiz secim"); printf("\n\n"); resetColor(); clearScreen();
            break;
    }
}

void sigortapolicesiislemleri(musteri m1) {
    clearScreen();
    setColor(14);
    printFrame("     SIGORTA POLICESI MENUSU     "); printf("\n\n");
    resetColor();

    setColor(11);
    printCentered("1- Arac Sigortasi"); printf("\n");
    printCentered("2- Konut Sigortasi"); printf("\n");
    printCentered("3- Saglik Sigortasi"); printf("\n\n");
    resetColor();
    setColor(10);
    printCentered("Seciminiz   : ");
    resetColor();


    int IslemSecim;
    setColor(12);
    scanf("%d", &IslemSecim);
    resetColor();

    switch (IslemSecim) {
        case 1: aracSigortasiSecimi(m1); break;
        case 2: konutSigortasiSecimi(m1); break;
        case 3: saglikSigortasiSecimi(m1); break;
        default:
           setColor(12); printf("\n\n"); printCentered("Gecersiz secim.."); resetColor(); printf("\n\n");
            break;
    }
}

void sifreDegistirTek(musteri m1){
    clearScreen();
    printf("\n\n");
    setColor(11); printCentered("%s Tek kullanimlik sifrenizi degistiriniz...",m1.adSoyad ); resetColor();
    int eskiSifre,yeni, sifreTekrar;
    printf("\n\n");
    setColor(10); printCentered("Eski Sifre        : ");resetColor();
    setColor(12); scanf("%d", &eskiSifre); printf("\n"); resetColor();
    setColor(10); printCentered("Yeni Sifre        : "); resetColor();
    setColor(12); scanf("%d", &yeni); printf("\n"); resetColor();
    setColor(10); printCentered("Yeni Sifre Tekrar : ");resetColor();
    setColor(12); scanf("%d", &sifreTekrar); printf("\n"); resetColor();
    int sonuc=0, sayac =0 ;
    if ( m1.sifre== eskiSifre && yeni == sifreTekrar )	// Eski sifre, yeni sifre ve tekrar sifre uygunsa devam et
    {
        musteri m2;
        // Musteri bilgilerini iceren dosyayi acma ve kontrol
        FILE *ptr= fopen("musteriler.dat","r+b");
        while( fread ( &m2, sizeof(musteri), 1, ptr ) !=NULL )
        {
            if ( strcmp( m1.tc, m2.tc ) ==0 )
            {
                sonuc=1;
                break;
            }
            sayac++;
        }
        if( sonuc==0)
        {
           setColor(12); printCentered("Sifre degistirme isleminde hata olustu !"); resetColor(); sleep(3);
            fclose(ptr);
        }
        else
        {
        	// Dosyayi basa alip, musterinin sifre durumunu degistirme ve yeni sifreyi yazma
            rewind(ptr);
            fseek ( ptr, (sayac) * sizeof(musteri), 0 ) ;
            m2.sifreDurum= 1;
            m2.sifre= yeni;
            fwrite ( &m2, sizeof(musteri), 1, ptr );
            fclose(ptr);

            printf("\n\n"); setColor(10); printCentered("Sifre degistirme isleminiz basarili, yeni sifreniz ile giris yapabilirsiniz");
            resetColor(); printf("\n\n"); sleep(6);
            fclose(ptr);
        }
    }
    else
    {
        setColor(12); printCentered("Hatali sifre degistirme islemi !"); printf("\n\n"); resetColor(); sleep(5);
    }
}

void musteriGiris(){
    int width = 200;
    clearScreen();

    setColor(14);
    printFrame("     MUSTERI GIRIS EKRANI     ");
    resetColor();

    char tc[20];
    int parola;

    setColor(10); printf("\n\n"); printCentered("TC      : "); resetColor();
    setColor(12);
    scanf(" %s", tc); resetColor();
    setColor(10); printf("\n\n");
    printCentered("PAROLA  : ");
    resetColor();
    setColor(12);
    scanf(" %d", &parola); resetColor();
    FILE *ptr=fopen("musteriler.dat","r+b");	// Musteri bilgilerini içeren dosyayi acma ve kontrol
    musteri m1;
    int sonuc=0;
	// Dosyadan musteri bilgilerini okuma ve kontrol
    while(fread ( &m1, sizeof(musteri), 1, ptr) !=NULL )
    {
        if( strcmp( tc, m1.tc) ==0 && parola == m1.sifre )
        {
            sonuc=1;
            break;
        }
    }
    fclose(ptr) ;
    if( sonuc==0)
    {
    	// Eger musteri bulunamazsa hatali giris mesaji yazdirma
    	setColor(12);
    	printf("\n\n");
        printCentered("TC veya Sifre Hatali !");
        printf("\n\n");
        sleep(1);
        printCentered("Ana Menuye Yonlendiriliyorsunuz...");
        printf("\n\n");
        sleep(3);
        resetColor();
        clearScreen();
    }
    else
    {
        if ( m1.sifreDurum==0 )	// Musterinin sifre durumuna gore islemler yapma
            sifreDegistirTek(m1) ;
            // Eger hesap dondurulmussa mesaj yazdirma ve ana menuye yonlendirme
            else if( m1.HesapDurum==0 )
            {
            setColor(12);
            printf("\n\n");
            printCentered("Hesabiniz dondurulmustur, Lutfen banka subeniz ile gorusunuz.... !");
            printf("\n\n");
            sleep(1);
            printCentered("Ana Menuye Yonlendiriliyorsunuz...... !");
            sleep(2);
            resetColor();
            clearScreen();
            }
        else
        musterimiz( m1 );
    }
}

int menu(){

    printf("\n\n\n\n\n\n\n\n");
    printf("                          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
    printf("                          ^^^^                                                         ^^^^\n");
    printf("                          ^^^^           @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@           ^^^^\n");
    printf("                          ^^^^           @                                 @           ^^^^\n");
    printf("                          ^^^^           @         GELISIM BANK            @           ^^^^\n");
    printf("                          ^^^^           @                                 @           ^^^^\n");
    printf("                          ^^^^           @         HOS GELDINIZ...         @           ^^^^\n");
    printf("                          ^^^^           @                                 @           ^^^^\n");
    printf("                          ^^^^           @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@           ^^^^\n");
    printf("                          ^^^^                                                         ^^^^\n");
    printf("                          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");




    sleep (5);

    int secim;

    clearScreen();

    setColor(14);
    printf("\n"); printFrame("             GELISIM BANK ATM             "); printf("\n\n");
    resetColor();

    setColor(11);
    printCentered("1- MUSTERI GIRIS"); printf("\n\n");
    printCentered("2- YETKILI GIRIS"); printf("\n\n");
    printCentered("0- PROGRAMI KAPAT"); printf("\n\n");
    resetColor();

    setColor(10);
    printCentered("Seciminiz   : ");
    resetColor();

    setColor(12);
    scanf(" %d", &secim);
    resetColor();

    return secim;

}

int main(){
    int secim = menu();

    while (secim != 0)
    {
        switch (secim)
        {
        case 1: musteriGiris(); break;
        case 2: yetkiliGiris(); break;
        case 0: break;
        default:
            setColor(12);
            printf("\n\n");
            printCentered("Gecersiz Secim Yapildi. Lutfen Tekrardan Yapmak Istediginiz Islemi Seciniz !");
            printf("\n\n");
            resetColor();
            sleep(5);
            break;

        }
        secim = menu();
    }
    setColor(11);
    printf("\n\n"); printCentered("Bizi tercih ettiginiz icin tesekkurler.");
    printf("\n\n"); printCentered("Iyi Gunler Dileriz...");
    printf("\n\n");
    resetColor();

    return 0;
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void setColor(int color) {
    #ifdef _WIN32
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    #endif
}

void resetColor() {
    #ifdef _WIN32
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // Reset to default color
    #endif
}

int getWindowWidth() {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    #else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return w.ws_col;
    #endif
}

void printCentered(const char *format, ...) {
    char buffer[256]; // Tampon boyutunu ihtiyacýnýza göre ayarlayýn
    va_list args;

    // Deðiþken parametre listesini baþlat
    va_start(args, format);

    // Parametreleri buffer içine yaz
    vsnprintf(buffer, sizeof(buffer), format, args);

    // Deðiþken parametre listesini sonlandýr
    va_end(args);

    // Ortalanmýþ metni hesapla ve yazdýr
    int width = 200; // veya dinamik olarak getWindowWidth() ile alabilirsiniz
    int len = strlen(buffer);
    int padding = (width - len) / 2;
    printf("%*s%s", padding, "", buffer);
}

void printFrame(char *text) {
    //int width = getWindowWidth();
    int width = 200;
    int len = strlen(text);
    int frameWidth = len + 4;
    int padding = (width - frameWidth) / 2;

    printf("%*s+", padding, "");
    for (int i = 0; i < frameWidth - 2; i++) printf("-");
    printf("+\n");

    printf("%*s| %s |\n", padding, "", text);

    printf("%*s+", padding, "");
    for (int i = 0; i < frameWidth - 2; i++) printf("-");
    printf("+\n");
}

