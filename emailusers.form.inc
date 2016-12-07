<?php


function _emailusers_form($form, &$form_state)
{


    // current user
    $current_user = user_load_by_name(get_current_user());


    $form = array();

    // Find users. Using filters to remove one-by-one. This makes it easy to define a rulesset.
    $users = array_filter(entity_load('user'), function ($item) {
        // skip admin
        if ($item->uid <= 1) return false;
        else return true;
    });


    // 'from name'
    $form['from'] = array(
        '#markup'=>t('<p>From: @from_name < @from_email >', array('@from_name'=>$current_user->realname, '@from_email'=> $current_user->mail)),
        '#attributers'=>array('class'=>array('emailuser_from'))
    );

    // field set
    $form['usergroup'] = array(
        '#type' => 'fieldset',
        '#title' => t('Users'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#attributes' => array(
            'class' => array('user-field-set')
        ),
    );

    // sort by name
    usort($users, function ($a, $b) {
        if ($a->realname == $b->realname) {
            return 0;
        }
        return $a->realname < $b->realname ? -1 : 1;
    });

    // loop users, put into field set
    foreach ($users as $u) {

        // get user roles to use as added roles to the check box
        $user_roles = array_filter($u->roles, '_emailusers_filter_userroles');

        $form['usergroup'][$u->uid] = array(
            '#type' => 'checkbox',
            '#title' => t($u->realname),
            '#attributes' => array('class' => $user_roles)
        );
    }


    $form['message'] = array(
        '#type' => 'text_format',
        '#title' => t('Message'),
    );


    $form['actions']['email'] = array(
        '#type' => 'submit',
        '#value' => t('Email'),
        '#submit' => array('_emailusers_submit_send'),
    );

    return $form;


}

/**
 * @param $form
 * @param $form_state
 *
 * Form Handler. This does the action that we're trying to do. This uses php's mail() function.
 */
function _emailusers_submit_send($form, &$form_state)
{

    // pull out users
    $selected_user_uids = array();
    foreach ($form_state['values'] as $k => $v) {
        if (is_numeric($k) && $v == 1) {
            $selected_user_uids[] = $k;
        }
    }

    // current user
    $current_user = user_load_by_name(get_current_user());

    // get basic properties
    // $from = variable_get('site_mail', false);
    $from = $current_user->mail;
    $subject = t(variable_get('emailusers_default_subj_line', 'Email from this site: ' . variable_get('site_name', '')));
    $body = $form_state['values']['message']['value'];

    watchdog('emailusers', t("Beginning email from @from_name"), array('@from_name'=>$current_user->realname));
    watchdog('emailusers', t("Message starts as: @msg"), array('@msg'=> substr($body, 0, 100)));


    // process selected users
    foreach (user_load_multiple($selected_user_uids) as $u) {

        $to = $u->mail;
        // simple_mail_queue($from, $to, $subject, $body);

        $headersfrom = 'MIME-Version: 1.0' . "\r\n";
        $headersfrom .= 'Content-type: text/html; charset=utf-8' . "\r\n";
        $headersfrom .= 'From: ' . $from . ' ' . "\r\n";

        $result = mail($to, $subject, $body, $headersfrom);


        if ($result) {
            $msg = t("A message was to be sent !to ", array('!to' => $to));
            drupal_set_message($msg);
            watchdog('emailuser', $msg);
        } else {
            $msg = t("The message to @name may have failed", array("@name"=>$u->realname));
            drupal_set_message($msg, 'warning');
            watchdog('emailuser', $msg);
        }
    }


    watchdog('emailuser', 'EOF');


}


/**
 * @param $item
 * @return bool
 *
 * Used as part of an array filter with returned roles or user roles. It allows a central point
 * to filter out basic roles that won't matter for UI interaction.
 */
function _emailusers_filter_userroles($item)
{
    switch ($item) {
        case 'authenticated user':
            return FALSE;
            break;
        case 'administrator':
            return FALSE;
            break;
        default:
            return TRUE;
    }
}
