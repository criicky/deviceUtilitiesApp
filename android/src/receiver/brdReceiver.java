package receiver;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkInfo;
import android.os.Build;
import android.net.LinkProperties;
import android.net.LinkAddress;
import android.net.wifi.WifiInfo;
import android.net.wifi.aware.WifiAwareNetworkInfo;
import android.net.NetworkRequest;
import android.telephony.TelephonyManager;
import android.app.Activity;
import android.Manifest;
import android.content.pm.PackageManager;
import android.os.UserManager;
import android.os.Bundle;
//import androidx.core.content.ContextCompat;
//import androidx.annotation.NonNull;

public class brdReceiver{

    public native void capabilities(WifiInfo wi,Network network,NetworkCapabilities networkCapabilities);
    public native void properties(Network network,LinkProperties linkProperties);
    public native void available(Network network);
    public native void lost(Network network);

    public void checkNetworkInfo(Context context, final OnConnectionStatusChange onConnectionStatusChange){
       
       ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE); //prendo il connectivity manager dal contensto
       UserManager uM = (UserManager) context.getSystemService(Context.USER_SERVICE);
       Bundle b = uM.getUserRestrictions();
       if(b != null && b.isEmpty()){
       	    System.out.println("Restricted");
       } 
       else{
            System.out.println("Not restricted");
       }
       if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {

            NetworkRequest request = new NetworkRequest.Builder().addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET).build();
            connectivityManager.registerNetworkCallback(request,new ConnectivityManager.NetworkCallback(1){	//metodo che viene chiamato finchè l applicazione esiste e vi sono cambiamenti nella connessione oppure finchè non eseguo un unregister callback
                
                @Override
                public void onLinkPropertiesChanged(Network network, LinkProperties linkProperties){
                    //properties(network,linkProperties);
                }
                
                @Override
                public void onCapabilitiesChanged(Network network, NetworkCapabilities networkCapabilities){
                    WifiInfo wi2 = (WifiInfo) networkCapabilities.getTransportInfo();
                    if(networkCapabilities.hasTransport(0)){
                    	System.out.println("cellular");
                    	/*if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_PHONE_STATE) != PackageManager.PERMISSION_GRANTED) {
    				// Permission not granted, request it
    				ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_PHONE_STATE}, PERMISSION_REQUEST_CODE);
			}*/
                    	//TelephonyManager tel = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
                    	//System.out.println("imei: " + tel.getDeviceId());
                    }
                    else if(networkCapabilities.hasTransport(1)){
                    	System.out.println("wifi");
                    }
                    //capabilities(wi2,network,networkCapabilities);
                }
                
                @Override
                public void onAvailable(Network network) {
                    //System.out.println("available " + network.toString());
                    available(network);
                }
                @Override
                public void onLost(Network network) {
                    //System.out.println("lost " + network.toString());
                    //lost(network);
                }
            });

        }
    }

    interface OnConnectionStatusChange{

        void onChange(boolean type);
    }
}
