import { Component } from '@angular/core';
import { NavController, ToastController } from 'ionic-angular';
import { Hotspot } from '@ionic-native/hotspot';
import { NativeStorage } from '@ionic-native/native-storage';
import { ServiceProvider } from '../../providers/service/service'
import { Validators, FormBuilder, FormGroup, FormControl } from '@angular/forms';
import { AlertController } from 'ionic-angular';

@Component({
  selector: 'page-wifi',
  templateUrl: 'wifi.html'
})
export class WiFiPage {

  wifi: boolean;
  ssid: string = 'Main-AP';
  password: string = '12345678';
  spassword: string = '12345678';
  passwordnew: string = '';
  status: string = '';
  ip: string = '192.168.4.1';
  bulbs = [];
  object: any;
  saveMode: boolean = false;
  change: boolean = false;

  passwordType: string = 'password';
  passwordIcon: string = 'eye-off';
  passwordType1: string = 'password';
  passwordIcon1: string = 'eye-off';
  constructor(
    private alertCtrl: AlertController,
    public navCtrl: NavController,
    public storage: NativeStorage,
    private hotspot: Hotspot,
    private http: ServiceProvider,
    public toastCtrl: ToastController,

  ) {
    this.hotspot.isAvailable().then((resolve) => {
      this.status += "<br>~Available";
    }).catch((error) => {
      this.status += "<br>~Not Available";
    });

    this.hotspot.isWifiSupported().then((resolve) => {
      this.status += "<br>~Wifi Supported";

      this.hotspot.isWifiOn().then((resolve) => {
        this.wifi = resolve;
      }).catch((error) => {
        this.status += "<br>~" + error;
      });
    }).catch((error) => {
      this.status += "<br>~Wifi Not Supported";
    });
    this.storage.getItem('password').then((result) => {
      this.status += "<br>~password stored=" + result;
      this.spassword = result;
    }).catch((error) => {
      // this.showToast('Error occured when loading password...');
    });

  }

  connectToWifi() {
    this.storage.setItem('ip', this.ip);
    this.hotspot.connectToWifi(this.ssid, this.password).then((resolve) => {
      this.status += "<br>~Success";
    }).catch((error) => {
      this.status += "<br>~" + error;
    });
  }

  getDetails() {
    this.http.get("http://" + this.ip + "/getDetails")
      .subscribe((observer) => {
        this.object = observer;
        this.bulbs = observer["res"];
        if (this.bulbs.length == 0){
          this.showToast("No bulb data records...");
        }else{
          this.showToast("Successfully updated bulb data...");
        }

      }, (error) => {
        this.status += "<br><br>~E~" + error;
        this.object = error;
      }, () => {
        this.status += "<br><br>~complete";
        let bulbArray = [];
        this.bulbs.forEach(bulb => {
          this.storage.getItem("bulbID-" + bulb.id).then((value) => {
            bulb["bulbName"] = value["bulbName"];
            bulb["bulbData"] = value["bulbData"];
            bulbArray.push(bulb);
          }).catch((error) => {
            this.showToast("Error occured...")
          });
        });
        this.bulbs = bulbArray;
      }
      );
  }

  switch(bulb) {
    let id = bulb.id;
    let state = bulb.state;
    if (this.saveMode) {
      this.saveIntensityAndSwitch(id, state);
    } else {
      this.directSwitch(id, state);
    }
  }

  directSwitch(id, state) {
    let strState = "false";
    if (state === 0) {
      strState = "true";
    }

    this.http.get("http://" + this.ip + "/changeBulbState?id=" + id + "&state=" + strState)
      .subscribe((observer) => {
        // this.showToast("Successfully chnaged...");
        this.getDetails();

      }, (error) => {
        this.status += "<br><br>~E~" + error;
        this.object = error;
      }, () => {
        this.status += "<br><br>~complete";
      }
      );
  }

  saveIntensityAndSwitch(id, state) {
    let strState = "false";
    if (state === 0) {
      strState = "true";
    }
    this.http.get("http://" + this.ip + "/getIntensity")
      .subscribe((observer) => {
        let intensity = observer["res"];
        let state = observer["state"];

        this.http.get("http://" + this.ip + "/setIntensity?id=" + id + "&intensity=" + intensity + "&isOn=" + strState)
          .subscribe((observer) => {
            // this.showToast("Successfully chnaged...");
            this.directSwitch(id, state);

          }, (error) => {
            this.status += "<br><br>~E~" + error;
            this.object = error;
          }, () => {
            this.status += "<br><br>~complete";
          }
          );
      }, (error) => {
        this.status += "<br><br>~E~" + error;
        this.object = error;
      }, () => {
        this.status += "<br><br>~complete";
      }
      );
  }

  toggleWifi() {
    this.hotspot.toggleWifi().then((resolve) => {
      this.wifi = !this.wifi;
    }).catch((error) => {
      this.status += "<br>~" + error;
    });
  }

  changepassword() {
    this.change = false;
    if (this.passwordnew.length == 8) {

      this.http.get("http://" + this.ip + "/changePass?password=" + this.passwordnew).subscribe((observer) => {
        this.status += "<br>~password " + observer;
      }, (err) => {
        this.status += "<br>~can not change password" + err;
        this.showAlert("connection error", "can not update password " + err);
      }, () => {
        this.status += "<br>~password has changed";
        this.storage.setItem("password", this.passwordnew
        ).then((resolve) => {
          this.showToast('Successfully stored...');
        }).catch((error) => {
          this.showToast('Error occured in storing...');
          this.showAlert("DB error", "can not store password " + error);
        });
      });

    } else {
      this.showAlert('Bad input', 'enter password with 8 characters');
    }
  }
  hideShowPassword() {

    this.passwordType = this.passwordType === 'text' ? 'password' : 'text';
    this.passwordIcon = this.passwordIcon === 'eye-off' ? 'eye' : 'eye-off';


  }
  hideShowPassword1() {
    this.passwordType1 = this.passwordType1 === 'text' ? 'password' : 'text';
    this.passwordIcon1 = this.passwordIcon1 === 'eye-off' ? 'eye' : 'eye-off';
  }
  getItems(ev: any) {
    var val = ev.target.value;
    console.log(val);
  }
  showAlert(Title, Mess) {
    const alert = this.alertCtrl.create({

      title: Title,
      subTitle: Mess,
      buttons: ['OK']
    });
    alert.present();
  }

  showToast(msg) {
    const toast = this.toastCtrl.create({
      message: msg,
      duration: 3000
    });
    toast.present();
  }
  changepass() {
    this.change = true;
  }
}
