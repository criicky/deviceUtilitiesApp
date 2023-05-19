package receiver;

import android.app.Activity;
import android.os.Bundle;
import android.content.Intent;
import android.content.Context;

public class mainReceiver extends Activity{ //devo trovare il modo di creare un istanza di questa classe da cpp e poi far mandare qualcosa da qui per prenderlo in cpp 

    public static Intent start(Context context){
            Intent intent = new Intent(context,mainReceiver.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            return intent;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	
    	System.out.println("mi sto creando");
        super.onCreate(savedInstanceState);

	brdReceiver brd = new brdReceiver();
        brd.checkNetworkInfo(this, new brdReceiver.OnConnectionStatusChange() {
        
            @Override
            public void onChange(boolean type) { //devo inserire una chiamata al mio progetto in cpp in modo da gestire il cambiamento
                if(type){
               		System.out.println("Ci sono connessioni");//se è già presente una connessione posso mandare un tipo di segnale
                }
                else{
                	System.out.println("Non ci sono connessioni");//se non vi è nessuna connessione allora devo chiamare lo scan e cercare di creare una nuova connessione
                }
            }
        });

    }
}
