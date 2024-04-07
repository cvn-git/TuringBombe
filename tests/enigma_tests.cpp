#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "enigma.h"

std::string removeSpaces(std::string_view input)
{
	std::string output;
	for(const char ch : input)
	{
		if(ch != ' ')
			output.push_back(ch);
	}
	return output;
}

struct EnigmaTestFixture
{
	bombe::ReflectorModel reflector_model;
	std::vector<bombe::RotorModel> rotor_models;
	std::string_view steckers;
	std::string_view ringstellung;
	std::string_view grundstellung;

	void runTest(std::string_view input, std::string_view output)
	{
		bombe::Enigma enigma(reflector_model, rotor_models);
		enigma.configureSteckers(steckers);
		enigma.configureRotors(ringstellung, grundstellung);

		const auto input_stripped = removeSpaces(input);
		const auto output_stripped = removeSpaces(output);
		std::string actual_output(input_stripped.size(), ' ');
		enigma.process(input_stripped, actual_output);
		DOCTEST_CHECK_EQ(output_stripped, actual_output);
	}
};

TEST_CASE_FIXTURE(EnigmaTestFixture, "Double step without steckers")
{
	// Example from my original enigma project
	reflector_model = bombe::ReflectorModel::THIN_C;
	rotor_models = {
		bombe::RotorModel::M_GAMMA, bombe::RotorModel::M_I, bombe::RotorModel::M_II, bombe::RotorModel::M_III};
	ringstellung = "VXYZ";
	grundstellung = "PQDV";

	runTest("The quick brown fox jumps over the lazy dog", "IRPMRQDZVBQIQDRDPCXGBAWODPBFIPISCAW");
}

TEST_CASE_FIXTURE(EnigmaTestFixture, "Double step with steckers")
{
	// Example from my original enigma project
	reflector_model = bombe::ReflectorModel::THIN_C;
	rotor_models = {
		bombe::RotorModel::M_GAMMA, bombe::RotorModel::M_I, bombe::RotorModel::M_II, bombe::RotorModel::M_III};
	steckers = "AB:CD:EF:GH";
	ringstellung = "VXYZ";
	grundstellung = "PQDV";

	runTest("The quick brown fox jumps over the lazy dog", "ICOMRQDZSAQIQMRCPDXHABWQCPWFIUISCBV");
}

TEST_CASE_FIXTURE(EnigmaTestFixture, "Rotor M_VII")
{
	// Rotor M_VII is rarely seen in examples available online, so I made one up (using Rijmenants' Enigma)
	reflector_model = bombe::ReflectorModel::THIN_C;
	rotor_models = {
		bombe::RotorModel::M_GAMMA, bombe::RotorModel::M_VI, bombe::RotorModel::M_VII, bombe::RotorModel::M_VIII};
	ringstellung = "RING";
	grundstellung = "GRUN";

	runTest("The quick brown fox jumps over the lazy dog", "PBDM QRNM SIGD WTKU WMCR BKNL PWRG RGGP TKI");
}

TEST_CASE_FIXTURE(EnigmaTestFixture, "Operation Barbarossa message #1")
{
	// http://wiki.franklinheath.co.uk/index.php/Enigma/Sample_Messages
	reflector_model = bombe::ReflectorModel::REGULAR_B;
	rotor_models = {bombe::RotorModel::M_II, bombe::RotorModel::M_IV, bombe::RotorModel::M_V};
	steckers = "AV:BS:CG:DL:FU:HZ:IN:KM:OW:RX";
	ringstellung = "BUL";
	grundstellung = "BLA";

	runTest(
		"EDPUD NRGYS ZRCXN UYTPO MRMBO FKTBZ REZKM LXLVE FGUEY SIOZV EQMIK UBPMM YLKLT TDEIS MDICA GYKUA CTCDO MOHWX "
		"MUUIA UBSTS LRNBZ SZWNR FXWFY SSXJZ VIJHI DISHP RKLKA YUPAD TXQSP INQMA TLPIF SVKDA SCTAC DPBOP VHJK",
		"AUFKL XABTE ILUNG XVONX KURTI NOWAX KURTI NOWAX NORDW ESTLX SEBEZ XSEBE ZXUAF FLIEG ERSTR ASZER IQTUN GXDUB "
		"ROWKI XDUBR OWKIX OPOTS CHKAX OPOTS CHKAX UMXEI NSAQT DREIN ULLXU HRANG ETRET ENXAN GRIFF XINFX RGTX");
}

TEST_CASE_FIXTURE(EnigmaTestFixture, "Operation Barbarossa message #2")
{
	// http://wiki.franklinheath.co.uk/index.php/Enigma/Sample_Messages
	reflector_model = bombe::ReflectorModel::REGULAR_B;
	rotor_models = {bombe::RotorModel::M_II, bombe::RotorModel::M_IV, bombe::RotorModel::M_V};
	steckers = "AV:BS:CG:DL:FU:HZ:IN:KM:OW:RX";
	ringstellung = "BUL";
	grundstellung = "LSD";

	runTest(
		"SFBWD NJUSE GQOBH KRTAR EEZMW KPPRB XOHDR OEQGB BGTQV PGVKB VVGBI MHUSZ YDAJQ IROAX SSSNR EHYGG RPISE ZBOVM "
		"QIEMM ZCYSG QDGRE RVBIL EKXYQ IRGIR QNRDN VRXCY YTNJR",
		"DREIG EHTLA NGSAM ABERS IQERV ORWAE RTSXE INSSI EBENN ULLSE QSXUH RXROE MXEIN SXINF RGTXD REIXA UFFLI EGERS "
		"TRASZ EMITA NFANG XEINS SEQSX KMXKM XOSTW XKAME NECXK");
}

TEST_CASE_FIXTURE(EnigmaTestFixture, "U-264 (Kapitänleutnant Hartwig Looks), 1942")
{
	// http://wiki.franklinheath.co.uk/index.php/Enigma/Sample_Messages
	reflector_model = bombe::ReflectorModel::THIN_B;
	rotor_models = {
		bombe::RotorModel::M_BETA, bombe::RotorModel::M_II, bombe::RotorModel::M_IV, bombe::RotorModel::M_I};
	steckers = "AT:BL:DF:GJ:HM:NW:OP:QY:RZ:VX";
	ringstellung = "AAAV";
	grundstellung = "VJNA";

	runTest(
		"NCZW VUSX PNYM INHZ XMQX SFWX WLKJ AHSH NMCO CCAK UQPM KCSM HKSE INJU SBLK IOSX CKUB HMLL XCSJ USRR DVKO HULX "
		"WCCB GVLI YXEO AHXR HKKF VDRE WEZL XOBA FGYU JQUK GRTV UKAM EURB VEKS UHHV OYHA BCJW MAKL FKLM YFVN RIZR VVRT "
		"KOFD ANJM OLBG FFLE OPRG TFLV RHOW OPBE KVWM UQFM PWPA RMFH AGKX IIBG",
		"VONV ONJL OOKS JHFF TTTE INSE INSD REIZ WOYY QNNS NEUN INHA LTXX BEIA NGRI FFUN TERW ASSE RGED RUEC KTYW ABOS "
		"XLET ZTER GEGN ERST ANDN ULAC HTDR EINU LUHR MARQ UANT ONJO TANE UNAC HTSE YHSD REIY ZWOZ WONU LGRA DYAC HTSM "
		"YSTO SSEN ACHX EKNS VIER MBFA ELLT YNNN NNNO OOVI ERYS ICHT EINS NULL");
}

TEST_CASE_FIXTURE(EnigmaTestFixture, "Scharnhorst (Konteradmiral Erich Bey), 1943")
{
	// http://wiki.franklinheath.co.uk/index.php/Enigma/Sample_Messages
	reflector_model = bombe::ReflectorModel::REGULAR_B;
	rotor_models = {bombe::RotorModel::M_III, bombe::RotorModel::M_VI, bombe::RotorModel::M_VIII};
	steckers = "AN:EZ:HK:IJ:LR:MQ:OT:PV:SW:UX";
	ringstellung = "AHM";
	grundstellung = "UZV";

	runTest("YKAEN ZAPMS CHZBF OCUVM RMDPY COFHA DZIZM EFXTH FLOLP ZLFGG BOTGO XGRET DWTJI QHLMX VJWKZ UASTR",
	        "STEUE REJTA NAFJO RDJAN STAND ORTQU AAACC CVIER NEUNN EUNZW OFAHR TZWON ULSMX XSCHA RNHOR STHCO");
}

TEST_CASE_FIXTURE(EnigmaTestFixture, "Nachricht P1030700")
{
	// https://enigma.hoerenberg.com/index.php?cat=The%20U534%20messages&page=P1030700
	reflector_model = bombe::ReflectorModel::THIN_B;
	rotor_models = {
		bombe::RotorModel::M_GAMMA, bombe::RotorModel::M_IV, bombe::RotorModel::M_III, bombe::RotorModel::M_VIII};
	steckers = "CH EJ NV OU TY LG SZ PK DI QB";
	ringstellung = "AACU";
	grundstellung = "VMGC";

	runTest("QBHEWTDFEQITKUWFQUHLIQQGVYGRSDOHDCOBFMDHXSKOFPAODRSVBEREIQZVEDAXSHOHBIYMCIIZSKGNDLNFKFVLWWHZXZGQXWSSPWLSOQ"
	        "XEANCELJYJCETZTLSTTWMTOBW",
	        "KOMXBDMXUUUBOOTEYFXDXUUUAUSBILVUNYYZWOSECHSXUUUFLOTTXVVVUUURWODREISECHSVIERKKREMASKKMITUUVZWODREIFUVFYEWHS"
	        "YUUUZWODREIFUNFZWOYUUFZWL");
}

TEST_CASE_FIXTURE(EnigmaTestFixture, "Nachricht P1030660")
{
	// https://enigma.hoerenberg.com/index.php?cat=The%20U534%20messages&page=P1030660
	reflector_model = bombe::ReflectorModel::THIN_C;
	rotor_models = {
		bombe::RotorModel::M_BETA, bombe::RotorModel::M_V, bombe::RotorModel::M_VI, bombe::RotorModel::M_VIII};
	steckers = "AE BF CM DQ HU JN LX PR SZ VW";
	ringstellung = "AAEL";
	grundstellung = "IGZQ";

	runTest("TWNHYAZGBILSHEWPGLBPQLWQEKITIAFGZHWIMCWDFXPAFEILQZWFNRFTTQHUOADVLRLGAOQKVLWLSJHWOFJJSLUVEYNRRAJAQDKQBGMFYC"
	        "EVKPFJPKOWHHQZYZEQRTQIKKXIXTFPOEMI",
	        "FXDXUUUOSTYFUNCQUUUFXWTTXVVVUUUEINSEINSNULDREIKKEISELEKKXXISTSECHSSTUENDLICHESDOCKENVORMITTAGSAMDREIXFUNFX"
	        "INRENDSBURGGEMXFXDXUUUOSTMOEGLICHL");
}
