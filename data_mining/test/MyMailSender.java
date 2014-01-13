package atrmat;

import java.util.Date;
import java.util.Properties;
import javax.mail.Authenticator;
import javax.mail.Message;
import javax.mail.MessagingException;
import javax.mail.PasswordAuthentication;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.AddressException;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;

/**
 * @see http://blog.sina.com.cn/s/blog_4e345ce70101b1x9.html
 * */
public class MyMailSender
{
    private String hostName = "smtp.gmail.com";
    private String userName = "atrftpmail@gmail.com";
    private String userPwd = "";
    private String fromUserMail = "atrftpmail@gmail.com";
    private String toUserMail = "";

    public void sendTestMail() throws AddressException, MessagingException
    {
        Properties mailProps = new Properties();
        mailProps.setProperty("mail.smtp.host", this.hostName);
        mailProps.setProperty("mail.smtp.starttls.enable", "true");
        mailProps.setProperty("mail.smtp.auth", "true");
        mailProps.setProperty("mail.smtp.quitwait", "false");
        // using the ssl protocol
        mailProps.setProperty("mail.smtp.socketFactory.class", "javax.net.ssl.SSLSocketFactory");   
        mailProps.setProperty("mail.smtp.socketFactory.fallback", "false");   
        mailProps.setProperty("mail.smtp.port", "465");   
        mailProps.setProperty("mail.smtp.socketFactory.port", "465"); 
        //如果不要对服务器的ssl证书进行受信任检查，测添加以下语句
        //mailProps.setProperty("mail.smtp.ssl.trust","*");

        Session mailSession = Session.getDefaultInstance(mailProps,
                new Authenticator()
                {
                    protected PasswordAuthentication getPasswordAuthentication()
                    {
                        return new PasswordAuthentication(userName, userPwd);
                    }
                });

        Message mailMessage = new MimeMessage(mailSession);
        mailMessage.setFrom(new InternetAddress(this.fromUserMail));
        mailMessage.setRecipients(Message.RecipientType.TO,
                InternetAddress.parseHeader(this.toUserMail, false));

        mailMessage.setSubject("This is a test mail.");
        mailMessage.setText("This is a test mail for gmail");
        mailMessage.setSentDate(new Date());

        Transport.send(mailMessage);
    }

    public static void main(String[] args)
    {
        try
        {
            new MyMailSender().sendTestMail();
        } 
        catch (AddressException e)
        {
            e.printStackTrace();
        } 
        catch (MessagingException e)
        {
            e.printStackTrace();
        }
    }
}
